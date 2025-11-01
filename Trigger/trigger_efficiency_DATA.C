#include <TFile.h>
#include <TTree.h>
#include <TH1F.h>
#include <TEfficiency.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <set>
#include <utility>

using json = nlohmann::json;

// ------------------------------------------------------------
// Cargar JSON (Golden JSON de CMS)
// ------------------------------------------------------------
std::set<std::pair<unsigned int, unsigned int>> LoadJSON(const char *jsonFile) {
    std::set<std::pair<unsigned int, unsigned int>> goodLumis;
    std::ifstream in(jsonFile);
    if (!in.is_open()) {
        std::cerr << "❌ No se pudo abrir el archivo JSON: " << jsonFile << std::endl;
        return goodLumis;
    }

    json j;
    in >> j;
    for (auto it = j.begin(); it != j.end(); ++it) {
        unsigned int run = std::stoul(it.key());
        for (auto &range : it.value()) {
            unsigned int start = range[0];
            unsigned int end = range[1];
            for (unsigned int l = start; l <= end; ++l) {
                goodLumis.insert({run, l});
            }
        }
    }

    std::cout << "✅ JSON cargado: " << goodLumis.size() << " pares (run, lumi) válidos.\n";
    return goodLumis;
}

// ------------------------------------------------------------
// Script principal
// ------------------------------------------------------------
void trigger_efficiency_final_fixed(const char *rootFile = "0290F73B-A51C-A441-AEC1-8429F9CC8AA8.root",
                                    const char *treeName = "Events",
                                    const char *jsonFile = "Cert_271036-284044_13TeV_Legacy2016_Collisions16_JSON.txt") {
    // Abrir archivo ROOT
    TFile *file = TFile::Open(rootFile);
    if (!file || file->IsZombie()) {
        std::cerr << "❌ Error al abrir archivo ROOT: " << rootFile << std::endl;
        return;
    }

    TTree *tree = (TTree*)file->Get(treeName);
    if (!tree) {
        std::cerr << "❌ No se encontró el árbol '" << treeName << "'." << std::endl;
        return;
    }

    std::cout << "✅ Archivo y árbol cargados correctamente.\n";

    // Cargar JSON
    auto goodLumis = LoadJSON(jsonFile);

    // Variables del árbol
    UInt_t run = 0;
    UInt_t lumi = 0;
    UInt_t nFatJet = 0;
    Float_t FatJet_pt[128];  // tamaño suficiente para todos los jets
    Bool_t HLT_AK8PFHT800_TrimMass50 = false;

    // SetBranchAddress
    tree->SetBranchAddress("run", &run);
    tree->SetBranchAddress("luminosityBlock", &lumi);
    tree->SetBranchAddress("nFatJet", &nFatJet);
    tree->SetBranchAddress("FatJet_pt", FatJet_pt);
//    tree->SetBranchAddress("HLT_AK8PFJet360_TrimMass30", &HLT_AK8PFJet360_TrimMass30);
    tree->SetBranchAddress("HLT_AK8PFHT800_TrimMass50", &HLT_AK8PFHT800_TrimMass50);


    Long64_t nentries = tree->GetEntries();
    std::cout << "🔹 Eventos totales: " << nentries << std::endl;

    // Histogramas
    TH1F *h_all = new TH1F("h_all", "FatJet pT (All);p_{T} [GeV];Eventos", 30, 200, 1200);
    TH1F *h_pass = new TH1F("h_pass", "FatJet pT (Trigger passed);p_{T} [GeV];Eventos", 30, 200, 1200);

    // Loop principal
    Long64_t nsel = 0;
    for (Long64_t i = 0; i < nentries; ++i) {
        if (i % 200000 == 0) std::cout << "Procesando evento " << i << " / " << nentries << std::endl;
        tree->GetEntry(i);

        // Filtrar por JSON
        if (!goodLumis.empty() && goodLumis.find({run, lumi}) == goodLumis.end()) continue;

        if (nFatJet < 1) continue;
        Float_t leading_pt = FatJet_pt[0];

        h_all->Fill(leading_pt);
        if (HLT_AK8PFHT800_TrimMass50) h_pass->Fill(leading_pt);
        nsel++;
    }

    std::cout << "✅ Eventos seleccionados (pasan JSON): " << nsel << std::endl;

    // Calcular eficiencia
    TCanvas *c1 = new TCanvas("c1", "Trigger Efficiency", 800, 700);
    gStyle->SetOptStat(0);
    c1->SetGrid();

    if (TEfficiency::CheckConsistency(*h_pass, *h_all)) {
        TEfficiency *eff = new TEfficiency(*h_pass, *h_all);
        eff->SetTitle("Eficiencia del trigger HLT_AK8PFHT800_TrimMass50;FatJet p_{T} [GeV];Eficiencia");
        eff->SetMarkerStyle(20);
        eff->SetMarkerColor(kBlue + 1);
        eff->SetLineColor(kBlue + 1);
        eff->Draw("AP");
    } else {
        std::cerr << "❌ Histogramas inconsistentes para TEfficiency.\n";
    }

    c1->SaveAs("trigger_efficiency_HLT_AK8PFHT800_TrimMass50.png");
    std::cout << "✅ Gráfico guardado: trigger_efficiency_HLT_AK8PFHT800_TrimMass50.png\n";
}

