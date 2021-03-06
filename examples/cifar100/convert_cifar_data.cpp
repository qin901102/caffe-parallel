//
// This script converts the CIFAR dataset to the leveldb format used
// by caffe to perform classification.
// Usage:
//    convert_cifar_data input_folder output_db_file
// The CIFAR dataset could be downloaded at
//    http://www.cs.toronto.edu/~kriz/cifar.html

#include <fstream>  // NOLINT(readability/streams)
#include <string>

#include "glog/logging.h"
#include "google/protobuf/text_format.h"
#include "leveldb/db.h"
#include "stdint.h"

#include "caffe/proto/caffe.pb.h"

using std::string;

const int kCIFARSize = 32;
const int kCIFARImageNBytes = 3072;

void read_image(std::ifstream* file, int* label, char* buffer) {
  char label_char;
  file->read(&label_char, 1); // the first is the coarse label, we do not use
  file->read(&label_char, 1);
  *label = label_char;
  file->read(buffer, kCIFARImageNBytes);
  return;
}

void convert_dataset(const string& input_folder, const string& output_folder) {
  // Leveldb options
  leveldb::Options options;
  options.create_if_missing = true;
  options.error_if_exists = true;
  // Data buffer
  int label;
  char str_buffer[kCIFARImageNBytes];
  string value;
  caffe::Datum datum;
  datum.set_channels(3);
  datum.set_height(kCIFARSize);
  datum.set_width(kCIFARSize);

  LOG(INFO) << "Writing Training data";
  leveldb::DB* train_db;
  leveldb::Status status;
  string output_file = output_folder + "/cifar100_train_leveldb";
  status = leveldb::DB::Open(options, output_file,
      &train_db);
  CHECK(status.ok()) << "Failed to open leveldb." << output_file;
  {
    string str_input_file = input_folder + "train.bin";
    std::ifstream data_file(str_input_file.c_str(),
        std::ios::in | std::ios::binary);
    CHECK(data_file) << "Unable to open train file #";
    for (int itemid = 0; itemid < 50000; ++itemid) {
      read_image(&data_file, &label, str_buffer);
      CHECK_LE(label, 99);
      CHECK_GE(label, 0);
      datum.set_label(label);
      datum.set_data(str_buffer, kCIFARImageNBytes);
      datum.SerializeToString(&value);
      snprintf(str_buffer, kCIFARImageNBytes, "%05d",
          itemid);
      train_db->Put(leveldb::WriteOptions(), string(str_buffer), value);
    }
  }

  LOG(INFO) << "Writing Testing data";
  leveldb::DB* test_db;
  CHECK(leveldb::DB::Open(options, output_folder + "/cifar100_test_leveldb",
      &test_db).ok()) << "Failed to open leveldb.";
  // Open files
  std::ifstream data_file((input_folder + "/test.bin").c_str(),
      std::ios::in | std::ios::binary);
  CHECK(data_file) << "Unable to open test file.";
  for (int itemid = 0; itemid < 10000; ++itemid) {
    read_image(&data_file, &label, str_buffer);
    datum.set_label(label);
    datum.set_data(str_buffer, kCIFARImageNBytes);
    datum.SerializeToString(&value);
    snprintf(str_buffer, kCIFARImageNBytes, "%05d", itemid);
    test_db->Put(leveldb::WriteOptions(), string(str_buffer), value);
  }

  delete train_db;
  delete test_db;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("This script converts the CIFAR dataset to the leveldb format used\n"
           "by caffe to perform classification.\n"
           "Usage:\n"
           "    convert_cifar_data input_folder output_folder\n"
           "Where the input folder should contain the binary batch files.\n"
           "The CIFAR dataset could be downloaded at\n"
           "    http://www.cs.toronto.edu/~kriz/cifar.html\n"
           "You should gunzip them after downloading.\n");
  } else {
    google::InitGoogleLogging(argv[0]);
    convert_dataset(string(argv[1]), string(argv[2]));
  }
  return 0;
}
