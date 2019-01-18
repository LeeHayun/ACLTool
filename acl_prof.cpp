#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include <vector>

double getTime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (double)tv.tv_sec + (double)1.0e-6*tv.tv_usec;
}

class IOperation
{
public:
  // Destructor
  virtual ~IOperation() = default;
  virtual void profile(TensorShape input_shape, TensorShape weights_shape, TensorShape output_shape);

private:

};

class ConvolutionLayerOperation final : public IOperation
{
public:
  ConvolutionLayerOperation();
  void profile(TensorShape input_shape, TensorShape weights_shape, TensorShape output_shape) override
  {

    TensorInfo info_input   = TensorInfo(input_shape, 1, DataType::F32, QuantizationInfo());
    TensorInfo info_weights = TensorInfo(input_shape, 1, DataType::F32, QuantizationInfo());
    TensorInfo info_output  = TensorInfo(input_shape, 1, DataType::F32, QuantizationInfo());

    info_input.set_data_layout   = DataLayout::NCHW;
    info_weights.set_data_layout = DataLayout::NCHW;
    info_output.set_data_layout  = DataLayout::NCHW;

    input.allocator()->init(info_input);
    weights.allocator()->init(info_weights);
    output.allocator()->init(info_output);

    convolution_layer.configure(

    

  }

private:
  CLConvolutionLayer convolution_layer;
  CLTensor           input;
  CLTensor           weights;
  CLTensor           output;
  CLTuner            tuner;
};

class ProfInfo
{
public:
  ProfInfo();

  void set_operation(char *operation)
  {
    if (!strcmp(operation, "ConvolutionLayer"))
    {
      _operation = new ConvolutionLayerOperation();
    }
  }

  void set_input_shape(char *input_shape)
  {
    int c, h, w;
    sscanf(input_shape, "%dx%dx%d", &w, &h, &c);
    _input_shape = TensorShape(w, h, c);
  }

  void set_output_shape(char *output_shape)
  {
    int c, h, w;
    sscanf(output_shape, "%dx%dx%d", &w, &h, &c);
    _output_shape = TensorShape(w, h, c);
  }

  void set_weights_shape(char *weights_shape)
  {
    int n, c, h, w;
    sscanf(weights_shape, "%dx%dx%dx%d", &w, &h, &c, &n);
    _weights_shape = TensorShape(w, h, c, n);
  }

  void set_activation(char *activation)
  {
    if (!strcmp(activation, "RELU"))
    {

    }
    else if (!strcmp(activation, "BOUNDED_RELU"))
    {
    }
  }

  void run()
  {
    operation->profile();
  }

private:
  IOperation* _operation;
  TensorShape _input_shape;
  TensorShape _output_shape;
  TensorShape _weights_shape;
};

class Profiler
{
public:
  Profiler();

  bool do_setup(int argc, char **argv)
  {
    //cmd_parser(argc, argv);
    filename = "test.txt";
    return true;
  }

  bool profile()
  {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
      fprintf(stderr, "Input file is not exist\n");
      return false;
    }

    char buf[256];
    while (fgets(fp, buf))
    {
      if (!strstr(buf, "Instantiated")
          continue;

      char *ptr = strtok(buf, " ");
      do {
        ptr = strtok(NULL, " ");
      } while (strstr(ptr, "Layer") == NULL);
      ProfInfo prof_info;
      prof_info.set_operation(ptr);
      
      while (ptr = strtok(NULL, " "))
      {
        if (!strncmp(ptr, "Input", 256))
        {
          ptr = strtok(NULL, " ");
          ptr = strtok(NULL, " ");
          prof_info.set_input_shape(ptr);
        }
        else if (!strncmp(ptr, "Weights", 256))
        {
          ptr = strtok(NULL, " ");
          ptr = strtok(NULL, " ");
          prof_info.set_weights_shape(ptr);
        }
        else if (!strncmp(ptr, "Shape:", 256))
        {
          ptr = strtok(NULL, " ");
          prof_info.set_input_shape(ptr);
        }
        else if (!strncmp(ptr, "Output", 256))
        {
          ptr = strtok(NULL, " ");
          ptr = strtok(NULL, " ");
          prof_info.set_output_shape(ptr);
        }
        //else if (!strncmp(ptr, "Epsilon", 256))
        //{
        //}
        else if (strstr(ptr, "RELU"))
        {
          prof_info.set_activation(ptr);
        }

      }

      v_prof_info.push_back(prof_info);
    }

    return true;
  }

  bool profile_success()
  {
    return true;
  }

  void print_results()
  {

  }

private:
  //CommmandLinerParser   cmd_parser;
  //CommonProfilerOptions common_opts;
  //CommonProfilerParams  common_params;
  std::string           filename;
  vector<ProfInfo>      v_prof_info;
};

int main(int argc, char **argv)
{
  // Initialize profiling tool
  Profiler profiler;

  // Profiler setup
  profiler.do_setup();

  // Run each layer for profiling
  profiler.profile();

  // Print profiling results
  if (profiler.profile_success())
  {
    profiler.print_results();
  }
  else
  {
    std::cout << "Profiling Fail" << std::endl;
  }

  return 0;
}
