#include <iostream>
#include <fstream>
#include <string>
#include "histos.pb.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"

//Structs to contain information to pass from ROOT to protobuf
struct histo_1D
{
  std::string hist_title;
  std::string yaxis_title;
  std::string xaxis_title;
  std::vector<std::string> bins;
  std::vector<float> bin_contents;
};

struct histo_2D
{
  std::string hist_title;
  std::string yaxis_title;
  std::string xaxis_title;
  std::vector<std::string> xbins;
  std::vector<std::string> ybins;
  std::vector<float> bin_contents;
};

//Functions to read ROOT information into structs
histo_1D Read1DHist(TH1 *hist)
{
  histo_1D output_hist;
  output_hist.hist_title = hist->GetTitle();
  output_hist.yaxis_title = hist->GetYaxis()->GetTitle();
  output_hist.xaxis_title = hist->GetXaxis()->GetTitle();

  for (int i = 0; i < hist->GetXaxis()->GetNbins(); i++)
  {
    std::string binlabel = hist->GetXaxis()->GetBinLabel(i);
    output_hist.bins.push_back(binlabel);

    float bincontent = hist->GetBinContent(i);
    output_hist.bin_contents.push_back(bincontent);
  }

  return output_hist;
}

histo_2D Read2DHist(TH2 *hist)
{
  histo_2D output_hist;
  output_hist.hist_title = hist->GetTitle();
  output_hist.yaxis_title = hist->GetYaxis()->GetTitle();
  output_hist.xaxis_title = hist->GetXaxis()->GetTitle();

  for (int i = 0; i < hist->GetXaxis()->GetNbins(); i++)
  {
    std::string binlabel = hist->GetXaxis()->GetBinLabel(i);
    output_hist.xbins.push_back(binlabel);
  }
  
  for (int i = 0; i < hist->GetYaxis()->GetNbins(); i++)
  {
    std::string binlabel = hist->GetYaxis()->GetBinLabel(i);
    output_hist.ybins.push_back(binlabel);
  }
  
  for (int i = 0; i < hist->GetNbinsX(); i++)
  {
    for (int j = 0; j < hist->GetNbinsY(); j++)
    {
      output_hist.bin_contents.push_back(hist->GetBinContent(i, j));
    }
  }

  return output_hist;
}

//Function to read in 1D histograms
void ReadHisto(histos::histo_1D* histo, std::string histo_name)
{
  //Read info from ROOT file into struct
  TFile *inputfile = new TFile("testplots.root");
  TH1 *root_hist;
  inputfile->GetObject(histo_name.c_str(), root_hist);

  histo_1D HistInfo = Read1DHist(root_hist);

  //Read info from structs into protobuf objects
  histo->set_hist_title(HistInfo.hist_title);
  histo->set_yaxis_title(HistInfo.yaxis_title);
  histo->set_xaxis_title(HistInfo.xaxis_title);

  for (int i = 0; i < HistInfo.bins.size(); i++)
  {
    histo->add_bins(HistInfo.bins[i]);
    histo->add_bin_contents(HistInfo.bin_contents[i]);
  }
}

void ReadHisto(histos::histo_2D* histo, std::string histo_name)
{
  //Read info from ROOT file into struct
  TFile *inputfile = new TFile("testplots.root");
  TH2 *root_hist;
  inputfile->GetObject(histo_name.c_str(), root_hist);

  histo_2D HistInfo = Read2DHist(root_hist);

  histo->set_hist_title(HistInfo.hist_title);

  histo->set_yaxis_title(HistInfo.yaxis_title);
  histo->set_xaxis_title(HistInfo.xaxis_title);

  std::cout << "Filling bins" << std::endl;
  for (int i = 0; i < HistInfo.xbins.size(); i++)
  {
    histo->add_xbins(HistInfo.xbins[i]);
  }
  for (int i = 0; i < HistInfo.ybins.size(); i++)
  {
    histo->add_ybins(HistInfo.ybins[i]);
  }
  for (int i = 0; i < HistInfo.bin_contents.size(); i++)
  {
    histo->add_bin_contents(HistInfo.bin_contents[i]);
  }
}

int main(int argc, char* argv[])
{
  //Verify library compatibility
  GOOGLE_PROTOBUF_VERIFY_VERSION;

if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " HISTO_FILE" << std::endl;
    return -1;
  } 

  std::cout << "Creating histogram collection" << std::endl;
  histos::hist_collection coll;
  std::cout << "Created" << std::endl;

  {
    //Read the existing collection
    std::fstream input(argv[1], std::ios::in | std::ios::binary);
    if (!input) 
    {
      std::cout << argv[1] << ": File not found. Creating new file." << std::endl;
    }
    else if (!coll.ParseFromIstream(&input))
    {
      std::cerr << "Failed to parse histogram collection." << std::endl;
      return -1;
    }
  }

  std::cout << "Reading histograms" << std::endl;
  ReadHisto(coll.add_histos_1d(), std::string("avg_height_1D") );
  ReadHisto(coll.add_histos_1d(), std::string("thresh_count")  );
  ReadHisto(coll.add_histos_2d(), std::string("avg_height_map"));
  std::cout << "Histogram reading complete" << std::endl;

  {
    std::fstream output(argv[1], std::ios::out | std::ios::trunc | std::ios::binary);
    if (!coll.SerializeToOstream(&output))
    {
      std::cerr << "Failed to write to histogram collection." << std::endl;
      return -1;
    }
  }

  //Delete all global objects allocated by libprotobuf.
  google::protobuf::ShutdownProtobufLibrary();
    
  return 0;
}
