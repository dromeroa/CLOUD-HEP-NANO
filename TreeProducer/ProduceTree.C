// Para correr  root -l -q 'ProduceTree.C("/code/TreeProducer","FinalTree.csv")':


#include <TFile.h>
#include <TTree.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#include <TVector2.h>
#include <TMath.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>

double deltaR(double eta1, double phi1, double eta2, double phi2) {
    double dEta = eta1 - eta2;
    double dPhi = TVector2::Phi_mpi_pi(phi1 - phi2);
    return TMath::Sqrt(dEta*dEta + dPhi*dPhi);
}

void ProduceTree(const std::string& inputDir, const std::string& outputCSV) {

    // Buscar archivos ROOT
    std::vector<std::string> inputFiles;
    for (const auto &entry : std::filesystem::directory_iterator(inputDir)) {
        if (entry.path().extension() == ".root") {
            inputFiles.push_back(entry.path());
        }
    }

    if (inputFiles.empty()) {
        std::cerr << "ERROR: No se encontraron archivos ROOT en " << inputDir << std::endl;
        return;
    }

    // Crear CSV
    std::ofstream csvFile(outputCSV);
    csvFile <<
        "nFatJet,pt,eta,msoftdrop,mass,"
        "tau1,tau2,tau3,tau21,tau32,"
        "jetId,rawFactor,subJetIdx1,subJetIdx2,"
        "deepTag_ZvsQCD,particleNet_HbbvsQCD,deepTagMD_WvsQCD,"
        "deepTag_WvsQCD,"
        "HLT_AK8PFJet450\n";

    // Loop sobre ROOTs
    for (const auto& file : inputFiles) {

        TFile *f = TFile::Open(file.c_str());
        if (!f || f->IsZombie()) {
            std::cerr << "ERROR: No se pudo abrir el archivo ROOT: " << file << std::endl;
            continue;
        }

        TTreeReader reader("Events", f);

        // HLT
        TTreeReaderValue<Bool_t> HLT_AK8PFJet450(reader, "HLT_AK8PFJet450");

        // FatJets
        TTreeReaderValue<UInt_t> nFatJet(reader, "nFatJet");
        TTreeReaderArray<Float_t> FatJet_pt(reader, "FatJet_pt");
        TTreeReaderArray<Float_t> FatJet_eta(reader, "FatJet_eta");
        TTreeReaderArray<Float_t> FatJet_phi(reader, "FatJet_phi");
        TTreeReaderArray<Float_t> FatJet_mass(reader, "FatJet_mass");
        TTreeReaderArray<Float_t> FatJet_msoftdrop(reader, "FatJet_msoftdrop");
        TTreeReaderArray<Float_t> FatJet_tau1(reader, "FatJet_tau1");
        TTreeReaderArray<Float_t> FatJet_tau2(reader, "FatJet_tau2");
        TTreeReaderArray<Float_t> FatJet_tau3(reader, "FatJet_tau3");
        TTreeReaderArray<Int_t>   FatJet_jetId(reader, "FatJet_jetId");
        TTreeReaderArray<Float_t> FatJet_rawFactor(reader, "FatJet_rawFactor");
        TTreeReaderArray<int>     FatJet_subJetIdx1(reader, "FatJet_subJetIdx1");
        TTreeReaderArray<int>     FatJet_subJetIdx2(reader, "FatJet_subJetIdx2");
        TTreeReaderArray<Float_t> FatJet_deepTag_ZvsQCD(reader, "FatJet_deepTag_ZvsQCD");
        TTreeReaderArray<Float_t> FatJet_particleNet_HbbvsQCD(reader, "FatJet_particleNet_HbbvsQCD");
        TTreeReaderArray<Float_t> FatJet_deepTagMD_WvsQCD(reader, "FatJet_deepTagMD_WvsQCD");
        TTreeReaderArray<Float_t> FatJet_deepTag_WvsQCD(reader, "FatJet_deepTag_WvsQCD");

        while (reader.Next()) {

            // FILTRO HLT
            if (!(*HLT_AK8PFJet450)) continue;

            // Loop FatJets
            for (UInt_t j = 0; j < *nFatJet; j++) {

                // CORTE PT > 450
                if (FatJet_pt[j] < 450) continue;

                float tau21 = (FatJet_tau1[j] > 0 ? FatJet_tau2[j] / FatJet_tau1[j] : -1);
                float tau32 = (FatJet_tau2[j] > 0 ? FatJet_tau3[j] / FatJet_tau2[j] : -1);

                // Escribir CSV
                csvFile
                    << *nFatJet << ","
                    << FatJet_pt[j] << ","
                    << FatJet_eta[j] << ","
                    << FatJet_msoftdrop[j] << ","
                    << FatJet_mass[j] << ","
                    << tau21 << ","
                    << tau32 << ","
                    << FatJet_jetId[j] << ","
                    << FatJet_rawFactor[j] << ","
                    << FatJet_subJetIdx1[j] << ","
                    << FatJet_subJetIdx2[j] << ","
                    << FatJet_deepTag_ZvsQCD[j] << ","
                    << FatJet_particleNet_HbbvsQCD[j] << ","
                    << FatJet_deepTagMD_WvsQCD[j] << ","
                    << FatJet_deepTag_WvsQCD[j] << ","
                    << (*HLT_AK8PFJet450 ? 1 : 0)
                    << "\n";
            }
        }

        f->Close();
    }

    csvFile.close();
    std::cout << "Procesamiento finalizado. CSV guardado en: " << outputCSV << std::endl;
}

