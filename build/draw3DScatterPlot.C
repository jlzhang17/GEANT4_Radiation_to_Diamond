#include "TFile.h"
#include "TTree.h"
#include "TGraph2D.h"
#include "TAxis.h"
void draw3DScatterPlot() {
    // 打开 ROOT 文件
    TFile *file = new TFile("Mydata.root", "READ");

    // 检查文件是否成功打开
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open the ROOT file." << std::endl;
        return;
    }

    // 从文件中获取数据的 TTree
    TTree *tree = dynamic_cast<TTree*>(file->Get("Mydata"));

    // 检查 TTree 是否成功获取
    if (!tree) {
        std::cerr << "Error: Could not retrieve the TTree from the file." << std::endl;
        file->Close();
        return;
    }

    // 设置 TBranchAddress，将 x_pos、y_pos、z_pos 列与变量关联
    double x_pos, y_pos, z_pos;
    tree->SetBranchAddress("x_pos", &x_pos);
    tree->SetBranchAddress("y_pos", &y_pos);
    tree->SetBranchAddress("z_pos", &z_pos);

    // 获取 TTree 的总条目数
    Long64_t totalEntries = tree->GetEntries();

    // 创建 TGraph2D 对象
    TGraph2D *graph2D = new TGraph2D();

    // 读取并填充 x_pos、y_pos、z_pos 列的数据到 TGraph2D
    for (Long64_t i = 0; i < totalEntries; ++i) {
        tree->GetEntry(i);
        graph2D->SetPoint(i, x_pos, y_pos, z_pos);
    }

    TAxis *xAxis = graph2D->GetXaxis();
    TAxis *yAxis = graph2D->GetYaxis();
    TAxis *zAxis = graph2D->GetZaxis();
    
    xAxis->SetTitle("X Axis");
    yAxis->SetTitle("Y Axis");
    zAxis->SetTitle("Z Axis");
    // 画三维散点图
   // graph2D->SetMarkerStyle(20);
    graph2D->SetMarkerColorAlpha(9, 0.35);
    graph2D->Draw("P0");

    // 关闭 ROOT 文件
    file->Close();
}

