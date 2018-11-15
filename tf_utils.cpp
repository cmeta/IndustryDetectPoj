#include "tf_utils.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>

#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4996)
#endif

static void DeallocateBuffer(void* data, size_t) {
  free(data);
}

static TF_Buffer* ReadBufferFromFile(const char* file) {
  FILE* f = fopen(file, "rb");  //以二进制模式读取
  if (f == nullptr) {  //空指针
    return nullptr;
  }

  fseek(f, 0, SEEK_END); //文件尾
  const long fsize = ftell(f); //得到文件长度
  fseek(f, 0, SEEK_SET); //文件头

  if (fsize < 1) {
    fclose(f);
    return nullptr;
  }

  void* data = malloc(fsize);
  fread(data, fsize, 1, f); //读1项fsize大小的数据
  fclose(f);

  TF_Buffer* buf = TF_NewBuffer();
  buf->data = data;
  buf->length = fsize;
  buf->data_deallocator = DeallocateBuffer;

  return buf;
}

TF_Graph* LoadGraphDef(const char* file) {
  TF_Buffer* buffer = ReadBufferFromFile(file);
  if (buffer == nullptr) {
    return nullptr;
  }

  TF_Graph* graph = TF_NewGraph();
  TF_Status* status = TF_NewStatus();
  TF_ImportGraphDefOptions* opts = TF_NewImportGraphDefOptions();

  TF_GraphImportGraphDef(graph, buffer, opts, status);
  TF_DeleteImportGraphDefOptions(opts);
  TF_DeleteBuffer(buffer);

  if (TF_GetCode(status) != TF_OK) {
    TF_DeleteStatus(status);
    TF_DeleteGraph(graph);
    return nullptr;
  }

  TF_DeleteStatus(status);

  return graph;
}

bool RunSession(TF_Graph* graph,
                TF_Output* input, TF_Tensor** input_tensor, int ninputs,
                TF_Output* output, TF_Tensor** output_tensor, int noutputs) {
  TF_Status* status = TF_NewStatus();
  TF_SessionOptions* options = TF_NewSessionOptions();
  TF_Session* sess = TF_NewSession(graph, options, status);
  TF_DeleteSessionOptions(options);

  if (TF_GetCode(status) != TF_OK) {
    TF_DeleteStatus(status);
    return false;
  }

  TF_SessionRun(sess,
                nullptr, // Run options.
                input, input_tensor, ninputs, // Input tensors, input tensor values, number of inputs.
                output, output_tensor, noutputs, // Output tensors, output tensor values, number of outputs.
                nullptr, 0, // Target operations, number of targets.
                nullptr, // Run metadata.
                status // Output status.
  );

  if (TF_GetCode(status) != TF_OK) {
    TF_DeleteStatus(status);
    return false;
  }

  TF_CloseSession(sess, status);
  if (TF_GetCode(status) != TF_OK) {
    TF_DeleteStatus(status);
    return false;
  }

  TF_DeleteSession(sess, status);
  if (TF_GetCode(status) != TF_OK) {
    TF_DeleteStatus(status);
    return false;
  }

  TF_DeleteStatus(status);

  return true;
}

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif
