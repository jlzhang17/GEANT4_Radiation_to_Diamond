void extract_data() {
    // === Step 1: Extract data from ROOT file ===
    TFile *file = TFile::Open("Mydata.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open file Mydata.root!" << std::endl;
        return;
    }

    TTree *tree = (TTree*)file->Get("Mydata");
    if (!tree) {
        std::cerr << "Error: Could not find tree 'Mydata'!" << std::endl;
        file->Close();
        return;
    }

    TTreePlayer *player = (TTreePlayer*)(tree->GetPlayer());
    player->SetScanRedirect(true);
    player->SetScanFileName("position_energy.txt");
    tree->Scan("x_pos:y_pos:z_pos:SKA_E");  // Original columns + PKA_E (assumed to be column 10) if u use QGSP_INCLXX_HP then remember to change 'PKA_E' to 'SKA_E'
    file->Close();

    // === Step 2: Filter the output file using awk ===
    // Filter rows where column 10 (PKA_E) is not zero
    gSystem->Exec("awk '$10 != 0' position_energy.txt > temp.txt && mv temp.txt position_energy.txt");

    // Filter consecutive duplicates in column 10 (PKA_E)
    //gSystem->Exec("awk 'NR==1 || $10 != prev {print} {prev = $10}' position_energy.txt > temp.txt && mv temp.txt position_energy.txt");

    std::cout << "Filtered data saved to position_energy.txt" << std::endl;
}

