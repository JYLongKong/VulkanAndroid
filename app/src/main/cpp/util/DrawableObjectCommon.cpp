#include "DrawableObjectCommon.h"
#include "mylog.h"
#include <assert.h>
#include "HelpFunction.h"
#include "MatrixState3D.h"
#include <string.h>

DrawableObjectCommon::DrawableObjectCommon(
    // 传入的顶点数据相关参数
    float *vdataIn,
    int dataByteCount,
    int vCountIn,

    /// Sample4_10、Sample4_16-传入的索引数据相关参数
//    uint16_t *idataIn,
//    int indexByteCount,
//    int iCountIn,

    VkDevice &device,
    VkPhysicalDeviceMemoryProperties &memoryroperties
) {
//  pushConstantData = new float[16];                                       // Sample4_2、6_1、6_7-推送常量数据数组的初始化(4X4的最终变换矩阵)
  pushConstantData = new float[32];                                       // Sample5_1、6_6、7_2
//  pushConstantDataVertex = new float[16];                                 // Sample6_5
//  pushConstantDataFrag = new float[1];                                    // Sample6_5

  this->devicePointer = &device;                                          // 接收逻辑设备指针并保存
  this->vdata = vdataIn;                                                  // 接收顶点数据数组首地址指针并保存
  this->vCount = vCountIn;                                                // 接收顶点数量并保存

  VkBufferCreateInfo buf_info = {};                                       // 构建缓冲创建信息结构体实例
  buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;                  // 设置结构体类型
  buf_info.pNext = nullptr;                                               // 自定义数据的指针
  buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;                     // 缓冲的用途为顶点数据
  buf_info.size = dataByteCount;                                          // 设置数据总字节数
  buf_info.queueFamilyIndexCount = 0;                                     // 队列家族数量(共享模式EXCLUSIVE时为0)
  buf_info.pQueueFamilyIndices = nullptr;                                 // 队列家族索引列表(共享模式EXCLUSIVE时为空)
  buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;                       // 共享模式(表示缓冲不允许被多个队列家族的队列访问)
  buf_info.flags = 0;                                                     // 标志
  VkResult result = vk::vkCreateBuffer(device, &buf_info, nullptr, &vertexDatabuf); // 创建缓冲
  assert(result == VK_SUCCESS);                                           // 检查缓冲创建是否成功

  // 注：缓冲创建后只是一种逻辑存在，Vulkan并没有自动为其在设备内存中开辟存储空间，还需要
  // 为缓冲分配匹配的设备内存，并将设备内存与缓冲绑定才真正确定缓冲在设备内存中的存储位置
  VkMemoryRequirements mem_reqs;                                          // 缓冲内存需求
  vk::vkGetBufferMemoryRequirements(device, vertexDatabuf, &mem_reqs);    // 获取缓冲内存需求
  assert(dataByteCount <= mem_reqs.size);                                 // 检查内存需求获取是否正确

  VkMemoryAllocateInfo alloc_info = {};                                   // 构建内存分配信息结构体实例
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;              // 结构体类型
  alloc_info.pNext = nullptr;                                             // 自定义数据的指针
  alloc_info.memoryTypeIndex = 0;                                         // 内存类型索引
  alloc_info.allocationSize = mem_reqs.size;                              // 内存总字节数
  VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT         // 需要的内存类型掩码
      | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;                             // 该组合表示分配的设备内存可以被CPU访问，同时保证CPU与GPU访问的一致性
  bool flag = memoryTypeFromProperties(                                   // 获取所需内存类型索引
      memoryroperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
  if (flag) {
    LOGI("confirm memory type success, memoryTypeIndex = %d", alloc_info.memoryTypeIndex);
  } else {
    LOGE("confirm memory type failed!");
  }
  result = vk::vkAllocateMemory(device, &alloc_info, nullptr, &vertexDataMem); // 为顶点数据缓冲分配内存
  assert(result == VK_SUCCESS);                                           // 检查内存分配是否成功

  // 将数据送入设备内存
  uint8_t *pData;                                                         // CPU访问时的辅助指针
  result = vk::vkMapMemory(device, vertexDataMem, 0, mem_reqs.size, 0, (void **) &pData); // 将设备内存映射为可供CPU访问
  assert(result == VK_SUCCESS);                                           // 检查映射是否成功
  memcpy(pData, vdata, dataByteCount);                                    // 将顶点数据(坐标、颜色)复制进设备内存
  vk::vkUnmapMemory(device, vertexDataMem);                               // 解除内存映射

  result = vk::vkBindBufferMemory(device, vertexDatabuf, vertexDataMem, 0); // 绑定内存与缓冲
  assert(result == VK_SUCCESS);

  vertexDataBufferInfo.buffer = vertexDatabuf;                            // 指定数据缓冲
  vertexDataBufferInfo.offset = 0;                                        // 数据缓冲起始偏移量
  vertexDataBufferInfo.range = mem_reqs.size;                             // 数据缓冲所占字节数

  /// Sample4_10、Sample4_16 上述被以下两个函数替代********************* start
//  this->idata = idataIn;                                                  // 接收索引数据数组首地址指针并保存
//  this->iCount = iCountIn;                                                // 接收索引数量并保存
//  createVertexBuffer(dataByteCount, device, memoryroperties);       // 调用方法创建顶点数据缓冲
//  createIndexBuffer(indexByteCount, device, memoryroperties);       // 调用方法创建索引数据缓冲
  /// Sample4_10、Sample4_16 **************************************** end

  /// Sample4_15、Sample4_16 ************************************** start
//  initDrawCmdbuf(device, memoryroperties);
  /// Sample4_15、Sample4_16 **************************************** end
}

/**
 * Sample4_10、Sample4_16
 * 创建顶点数据缓冲的方法
 */
void DrawableObjectCommon::createVertexBuffer(int dataByteCount,
                                              VkDevice &device,
                                              VkPhysicalDeviceMemoryProperties &memoryroperties) {
  VkBufferCreateInfo buf_info = {};                                       // 构建缓冲创建信息结构体实例，为创建顶点数据缓冲服务
  buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;                  // 设置结构体类型
  buf_info.pNext = nullptr;                                               // 自定义数据的指针
  buf_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;                     // 缓冲的用途为顶点数据
  buf_info.size = dataByteCount;                                          // 设置数据总字节数
  buf_info.queueFamilyIndexCount = 0;                                     // 队列家族数量(共享模式EXCLUSIVE时为0)
  buf_info.pQueueFamilyIndices = nullptr;                                 // 队列家族索引列表(共享模式EXCLUSIVE时为空)
  buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;                       // 共享模式(表示缓冲不允许被多个队列家族的队列访问)
  buf_info.flags = 0;                                                     // 标志
  VkResult result = vk::vkCreateBuffer(device, &buf_info, nullptr, &vertexDatabuf); // 创建缓冲
  assert(result == VK_SUCCESS);                                           // 检查缓冲创建是否成功

  // 注：缓冲创建后只是一种逻辑存在，Vulkan并没有自动为其在设备内存中开辟存储空间，还需要
  // 为缓冲分配匹配的设备内存，并将设备内存与缓冲绑定才真正确定缓冲在设备内存中的存储位置
  VkMemoryRequirements mem_reqs;                                          // 缓冲内存需求
  vk::vkGetBufferMemoryRequirements(device, vertexDatabuf, &mem_reqs);    // 获取缓冲内存需求
  assert(dataByteCount <= mem_reqs.size);                                 // 检查内存需求获取是否正确

  VkMemoryAllocateInfo alloc_info = {};                                   // 构建内存分配信息结构体实例
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;              // 结构体类型
  alloc_info.pNext = nullptr;                                             // 自定义数据的指针
  alloc_info.memoryTypeIndex = 0;                                         // 内存类型索引
  alloc_info.allocationSize = mem_reqs.size;                              // 内存总字节数
  VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT         // 需要的内存类型掩码
      | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;                             // 该组合表示分配的设备内存可以被CPU访问，同时保证CPU与GPU访问的一致性
  bool flag = memoryTypeFromProperties(                                   // 获取所需内存类型索引
      memoryroperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
  if (flag) {
    LOGI("confirm memory type success, memoryTypeIndex = %d", alloc_info.memoryTypeIndex);
  } else {
    LOGE("confirm memory type failed!");
  }
  result = vk::vkAllocateMemory(device, &alloc_info, nullptr, &vertexDataMem); // 为顶点数据缓冲分配内存
  assert(result == VK_SUCCESS);                                           // 检查内存分配是否成功

  // 将数据送入设备内存
  uint8_t *pData;                                                         // CPU访问时的辅助指针
  result = vk::vkMapMemory(device, vertexDataMem, 0, mem_reqs.size, 0, (void **) &pData); // 将显存映射为可供CPU访问
  assert(result == VK_SUCCESS);                                           // 检查映射是否成功
  memcpy(pData, vdata, dataByteCount);                                    // 将顶点数据(坐标、颜色)复制进显存
  vk::vkUnmapMemory(device, vertexDataMem);                               // 解除内存映射

  result = vk::vkBindBufferMemory(device, vertexDatabuf, vertexDataMem, 0); // 绑定内存与缓冲
  assert(result == VK_SUCCESS);

  // 记录Buffer Info
  vertexDataBufferInfo.buffer = vertexDatabuf;                            // 指定数据缓冲
  vertexDataBufferInfo.offset = 0;                                        // 数据缓冲起始偏移量
  vertexDataBufferInfo.range = mem_reqs.size;                             // 数据缓冲所占字节数
}

/**
 * Sample4_10、Sample4_16
 * 创建索引数据缓冲的方法
 */
void DrawableObjectCommon::createIndexBuffer(int indexByteCount,
                                             VkDevice &device,
                                             VkPhysicalDeviceMemoryProperties &memoryroperties) {
  // 创建Buffer创建信息实例，为创建索引数据Buffer服务
  VkBufferCreateInfo index_buf_info = {};
  index_buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  index_buf_info.pNext = nullptr;
  index_buf_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;                // 索引缓冲用法
  index_buf_info.size = indexByteCount;
  index_buf_info.queueFamilyIndexCount = 0;
  index_buf_info.pQueueFamilyIndices = nullptr;
  index_buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  index_buf_info.flags = 0;
  VkResult result = vk::vkCreateBuffer(device, &index_buf_info, nullptr, &indexDatabuf);
  assert(result == VK_SUCCESS);

  // 获取内存需求
  VkMemoryRequirements index_mem_reqs;
  vk::vkGetBufferMemoryRequirements(device, indexDatabuf, &index_mem_reqs);
  assert(indexByteCount <= index_mem_reqs.size);

  // 内存分配信息
  VkMemoryAllocateInfo index_alloc_info = {};
  index_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  index_alloc_info.pNext = nullptr;
  index_alloc_info.memoryTypeIndex = 0;
  index_alloc_info.allocationSize = index_mem_reqs.size;

  // 需要的内存类型掩码
  VkFlags index_requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

  // 获取所需内存类型索引
  bool index_flag = memoryTypeFromProperties(
      memoryroperties, index_mem_reqs.memoryTypeBits, index_requirements_mask, &index_alloc_info.memoryTypeIndex);
  if (index_flag) {
    LOGI("confirm index-memory type success, memoryTypeIndex = %d", index_alloc_info.memoryTypeIndex);
  } else {
    LOGE("confirm index-memory type failed!");
  }
  // 为索引数据缓冲分配内存
  result = vk::vkAllocateMemory(device, &index_alloc_info, nullptr, &indexDataMem);
  assert(result == VK_SUCCESS);

  uint8_t *index_pData;
  // 将显存映射为CPU可访问
  result = vk::vkMapMemory(device, indexDataMem, 0, index_mem_reqs.size, 0, (void **) &index_pData);
  assert(result == VK_SUCCESS);

  // 将索引数据拷贝进显存
  memcpy(index_pData, idata, indexByteCount);
  // 解除内存映射
  vk::vkUnmapMemory(device, indexDataMem);

  // 绑定内存与缓冲
  result = vk::vkBindBufferMemory(device, indexDatabuf, indexDataMem, 0);
  assert(result == VK_SUCCESS);

  // 记录Buffer Info
  indexDataBufferInfo.buffer = indexDatabuf;
  indexDataBufferInfo.offset = 0;
  indexDataBufferInfo.range = index_mem_reqs.size;
}

/**
 * Sample4_15、Sample4_16
 * 用于创建间接绘制信息数据缓冲的方法
 */
void DrawableObjectCommon::initDrawCmdbuf(VkDevice &device, VkPhysicalDeviceMemoryProperties &memoryroperties) {
//  indirectDrawCount = 1;                                                  // Sample4_15-间接绘制信息数据组的数量
  indirectDrawCount = 2;                                                  // Sample4_16-间接绘制信息数据组的数量
  drawCmdbufbytes = indirectDrawCount * sizeof(VkDrawIndirectCommand);    // 信息数据所占总字节数

  VkBufferCreateInfo buf_info = {};                                       // 构建缓冲创建信息结构体实例
  buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buf_info.pNext = nullptr;
  buf_info.usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;                   // 设置缓冲用途为间接绘制信息数据缓冲
  buf_info.size = drawCmdbufbytes;                                        // 设置数据总字节数
  buf_info.queueFamilyIndexCount = 0;                                     // 队列家族数量
  buf_info.pQueueFamilyIndices = nullptr;                                 // 队列家族列表
  buf_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;                       // 共享模式
  buf_info.flags = 0;                                                     // 标志

  VkResult result = vk::vkCreateBuffer(device, &buf_info, nullptr, &drawCmdbuf);  // 创建缓冲
  assert(result == VK_SUCCESS);

  VkMemoryRequirements mem_reqs;                                          // 缓冲内存需求
  vk::vkGetBufferMemoryRequirements(device, drawCmdbuf, &mem_reqs);       // 获取缓冲内存需求
  assert(drawCmdbufbytes <= mem_reqs.size);                               // 检查内存需求获取是否正确

  VkMemoryAllocateInfo alloc_info = {};                                   // 构建内存分配信息结构体实例
  alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  alloc_info.pNext = nullptr;
  alloc_info.memoryTypeIndex = 0;                                         // 内存类型索引
  alloc_info.allocationSize = mem_reqs.size;                              // 内存总字节数

  VkFlags requirements_mask = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT         // 需要的内存类型掩码
      | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  bool flag = memoryTypeFromProperties(                                   // 获取所需内存类型索引
      memoryroperties, mem_reqs.memoryTypeBits, requirements_mask, &alloc_info.memoryTypeIndex);
  if (flag) {
    LOGI("Determining memory type succeeded, alloc_info.memoryTypeIndex=%d", alloc_info.memoryTypeIndex);
  } else {
    LOGE("Determining memory type failed!");
  }

  result = vk::vkAllocateMemory(device, &alloc_info, nullptr, &drawCmdMem); // 为缓冲分配内存
  assert(result == VK_SUCCESS);

  uint8_t *pData;                                                         // CPU访问时的辅助指针
  result = vk::vkMapMemory(device, drawCmdMem, 0, mem_reqs.size, 0, (void **) &pData); // 将设备内存映射为CPU可访问
  assert(result == VK_SUCCESS);

  /// Sample4_15 ************************************************* start
  // VkDrawIndirectCommand的参数含义与vk::vkCmdDraw方法参数含义相同
//  VkDrawIndirectCommand dic;                                              // 构建间接绘制信息结构体实例
//  dic.vertexCount = vCount;                                               // 顶点数量
//  dic.firstInstance = 0;                                                  // 第一个绘制的实例序号
//  dic.firstVertex = 0;                                                    // 第一个绘制用的顶点索引
//  dic.instanceCount = 1;                                                  // 需要绘制的实例数量
  /// Sample4_15 *************************************************** end

  /// Sample4_16 ************************************************* start
  VkDrawIndexedIndirectCommand dic[2];                                    // 创建间接绘制信息结构体实例数组
  dic[0].indexCount = iCount;                                             // 第1组绘制信息数据的索引数量
  dic[0].instanceCount = 1;                                               // 第1组绘制信息数据的实例数量
  dic[0].firstIndex = 0;                                                  // 第1组绘制信息数据的绘制用起始索引
  dic[0].vertexOffset = 0;                                                // 第1组绘制信息数据的顶点数据偏移量
  dic[0].firstInstance = 0;                                               // 第1组绘制信息数据的首实例索引
  dic[1].indexCount = iCount / 2 + 1;                                     // 第2组绘制信息数据的索引数量
  dic[1].instanceCount = 1;                                               // 第2组绘制信息数据的实例数量
  dic[1].firstIndex = 0;                                                  // 第2组绘制信息数据的绘制用起始索引
  dic[1].vertexOffset = 0;                                                // 第2组绘制信息数据的顶点数据偏移量
  dic[1].firstInstance = 0;                                               // 第2组绘制信息数据的首实例索引
  /// Sample4_16 *************************************************** end

  memcpy(pData, &dic, drawCmdbufbytes);                                   // 将数据拷贝进设备内存
  vk::vkUnmapMemory(device, vertexDataMem);                               // 解除内存映射

  result = vk::vkBindBufferMemory(device, drawCmdbuf, drawCmdMem, 0);     // 绑定内存与缓冲
  assert(result == VK_SUCCESS);
}

DrawableObjectCommon::~DrawableObjectCommon() {
  delete[] vdata;                                                         // 释放顶点数据内存
  delete[] pushConstantData;                                              // Sample4_2

  /// Sample6_5 ************************************************** start
//  delete[] pushConstantDataVertex;
//  delete[] pushConstantDataFrag;
  /// Sample6_5 **************************************************** end

  vk::vkDestroyBuffer(*devicePointer, vertexDatabuf, nullptr);            // 销毁顶点数据缓冲
  vk::vkFreeMemory(*devicePointer, vertexDataMem, nullptr);               // 释放顶点数据缓冲对应设备内存

  /// Sample4_10、Sample4_16 ************************************* start
//  delete[] idata;                                                         // 释放索引数据内存
//  vk::vkDestroyBuffer(*devicePointer, indexDatabuf, nullptr);             // 销毁索引数据缓冲
//  vk::vkFreeMemory(*devicePointer, indexDataMem, nullptr);                // 释放索引数据缓冲对应设备内存
  /// Sample4_10、Sample4_16 *************************************** end

  /// Sample4_15、Sample4_16 ************************************* start
//  vk::vkDestroyBuffer(*devicePointer, drawCmdbuf, nullptr);
//  vk::vkFreeMemory(*devicePointer, drawCmdMem, nullptr);
  /// Sample4_15、Sample4_16 *************************************** end
}

/**
 * 绘制物体
 */
void DrawableObjectCommon::drawSelf(
    VkCommandBuffer &cmd,
    VkPipelineLayout &pipelineLayout,
    VkPipeline &pipeline,
    VkDescriptorSet *desSetPointer

    /// Sample4_10
//    uint32_t sIndex,
//    uint32_t eIndex

    /// Sample4_16
//    int cmdDataOffset

    /// Sample6_5
//    float lodLevel

    /// Sample6_10
//    int texArrayIndex
) {
  // VK_PIPELINE_BIND_POINT_GRAPHICS表示绑定的管线为图形渲染管线
  vk::vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);  // 将当前使用的命令缓冲与指定管线绑定
  vk::vkCmdBindDescriptorSets(                                            // 将命令缓冲、管线布局、描述集绑定
      cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, desSetPointer, 0, nullptr);
  const VkDeviceSize offsetsVertex[1] = {0};                              // 顶点数据偏移量数组
  vk::vkCmdBindVertexBuffers(                                             // 将顶点数据与当前使用的命令缓冲绑定
      cmd,                                                                // 当前使用的命令缓冲
      0,                                                                  // 顶点数据缓冲在列表中的首索引
      1,                                                                  // 绑定顶点缓冲的数量
      &(vertexDatabuf),                                                   // 绑定的顶点数据缓冲列表
      offsetsVertex                                                       // 各个顶点数据缓冲的内部偏移量
  );

  /// Sample4_2、6_1、6_7、6_10 ********************************** start
//  float *mvp = MatrixState3D::getFinalMatrix();                           // 获取最终变换矩阵
//  memcpy(pushConstantData, mvp, sizeof(float) * 16);           // 将最终变换矩阵复制进推送常量数据数组
////  pushConstantData[16] = texArrayIndex;                                   // Sample6_10-将纹理数组索引数据送入推送常量数据
//  vk::vkCmdPushConstants(cmd, pipelineLayout,                             // 将推送常量数据送入管线
//                         VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, pushConstantData);
////                         VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 17, pushConstantData); // Sample6_10
  /// Sample4_2、6_1、6_7、6_10 ************************************ end

  /// Sample5_2、6_6、7_2 **************************************** start
  float *mvp = MatrixState3D::getFinalMatrix();
  float *mm = MatrixState3D::getMMatrix();
  memcpy(pushConstantData, mvp, sizeof(float) * 16);
  memcpy(pushConstantData + 16, mm, sizeof(float) * 16);
  vk::vkCmdPushConstants(cmd, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 32, pushConstantData);
  /// Sample5_2、6_6、7_2 ****************************************** end

  /// Sample6_5 ************************************************** start
//  float *mvp = MatrixState3D::getFinalMatrix();
//  memcpy(pushConstantDataVertex, mvp, sizeof(float) * 16);
//  vk::vkCmdPushConstants(cmd, pipelineLayout,
//                         VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 16, pushConstantDataVertex);
//  pushConstantDataFrag[0] = lodLevel;                                     // 纹理采样细节级别数据
//  vk::vkCmdPushConstants(cmd, pipelineLayout,                             // 将纹理采样细节级别数据送入推送常量
//                         VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(float) * 16, sizeof(float) * 1, pushConstantDataFrag);
  /// Sample6_5 **************************************************** end

  vk::vkCmdDraw(cmd, vCount, 1, 0, 0);                                    // 执行绘制

  /// Sample4_10 ************************************************* start
//  vk::vkCmdBindIndexBuffer(                                               // 将索引数据与当前使用的命令缓冲绑定
//      cmd,                                                                // 当前使用的命令缓冲
//      indexDatabuf,                                                       // 索引数据缓冲
//      0,                                                                  // 索引数据缓冲首索引
//      VK_INDEX_TYPE_UINT16                                                // 索引数据类型
//  );
//  vk::vkCmdDrawIndexed(                                                   // 执行索引绘制
//      cmd,                                                                // 当前使用的命令缓冲
//      eIndex - sIndex,                                                    // 索引数量
//      1,                                                                  // 需要绘制的实例数量
//      sIndex,                                                             // 绘制用起始索引
//      0,                                                                  // 顶点数据偏移量
//      0                                                                   // 需要绘制的第1个实例的索引
//  );
  /// Sample4_10 *************************************************** end

  /// Sample4_15 ************************************************* start
//  vk::vkCmdDrawIndirect(
//      cmd,                                                                // 当前使用的命令缓冲
//      drawCmdbuf,                                                         // 间接绘制信息数据缓冲
//      0,                                                                  // 绘制信息数据的起始偏移量
//      indirectDrawCount,                                                  // 此次绘制使用的间接绘制信息组的数量
//      sizeof(VkDrawIndirectCommand));                                     // 每组绘制信息数据所占字节数
  /// Sample4_15 *************************************************** end

  /// Sample4_16 ************************************************* start
//  vk::vkCmdBindIndexBuffer(cmd, indexDatabuf, 0, VK_INDEX_TYPE_UINT16);
//  vk::vkCmdDrawIndexedIndirect(
//      cmd,                                                                // 当前使用的命令缓冲
//      drawCmdbuf,                                                         // 间接绘制信息数据缓冲
//      cmdDataOffset,                                                      // 绘制信息数据的起始偏移量(以字节计)
//      1,                                                                  // 此次绘制使用的间接绘制信息组的数量
//      sizeof(VkDrawIndexedIndirectCommand)                                // 每组绘制信息数据所占字节数
//  );
  /// Sample4_16 *************************************************** end
}
