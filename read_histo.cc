#include <iostream>
#include <fstream>
#include <string>
#include "histos.pb.h"

//Iterates through histos in the collection and prints their info
void ListHistos(const histos::hist_collection coll)
{
  for (int i = 0; i < coll.histos_1d_size(); i++)
  {
    const histos::histo_1D& hist = coll.histos_1d(i);
    std::cout << "Hist title: " << hist.hist_title() << std::endl;
    std::cout << "Y axis: " << hist.yaxis_title() << std::endl;
    std::cout << "X axis: " << hist.xaxis_title() << std::endl;
    std::cout << "Bins: " << std::endl;
    for (int j = 0; j < hist.bins_size(); j++)
    {
      std::cout << hist.bins(j) << ": " << hist.bin_contents(j) << " entries" << std::endl;
    }
  }
  
  for (int i = 0; i < coll.histos_2d_size(); i++)
  {
    const histos::histo_2D& hist = coll.histos_2d(i);
    std::cout << "Hist title: " << hist.hist_title() << std::endl;
    std::cout << "Y axis: " << hist.yaxis_title() << std::endl;
    std::cout << "X axis: " << hist.xaxis_title() << std::endl;
    std::cout << "Bins: " << std::endl;

    int bin_index = 0;
    for (int j = 0; j < hist.xbins_size(); j++)
    {
      for (int k = 0; k < hist.ybins_size(); k++)
      {
        std::cout << hist.xbins(j) << ", " << hist.ybins(k) << " : " << hist.bin_contents(bin_index) << std::endl;
        bin_index++;
      }
    }



  }
}

//Main function: reads the hist collection from a file and prints all info inside.

int main(int argc, char* argv[])
{
  //Verify library compatibility
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << "HISTO_FILE" << std::endl;
    return -1;
  }

  histos::hist_collection coll;

  {
    std::fstream input(argv[1], std::ios::in | std::ios::binary);
    if (!coll.ParseFromIstream(&input))
    {
      std::cerr << "Failed to parse histogram collection" << std::endl;
      return -1;
    }
  }

  ListHistos(coll);

  //Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();
    
  return 0;
}
