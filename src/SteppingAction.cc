//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file SteppingAction.cc
/// \brief Implementation of the B1::SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include <fstream>
#include <vector>
#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "Analysis.hh"
#include "G4VSolid.hh"
#include "G4StepPoint.hh"
#include "G4ThreeVector.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4IonTable.hh"
using namespace std;
namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* eventAction)
: fEventAction(eventAction)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!fScoringVolume) {
    const DetectorConstruction* detConstruction
      = static_cast<const DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume = detConstruction->GetScoringVolume();
  }

  // get volume of the current step
  G4LogicalVolume* volume
    = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();

  // check if we are in scoring volume
  if (volume != fScoringVolume) return;
  //如果presteppoint所在的volume是定义的fScoringVolume则输入数据
  //
  if (volume == fScoringVolume){
	  if (step->IsLastStepInVolume()) {
    // 粒子飞出了边界
    // 可以在这里执行相应的操作 
  G4cout<< " x_boundary: " << step->GetPostStepPoint()->GetPosition().x()/CLHEP::cm <<" cm" << G4endl;
  G4cout<< " y_boundary: " << step->GetPostStepPoint()->GetPosition().y()/CLHEP::cm <<" cm" << G4endl;
  G4cout<< " z_boundary: " << step->GetPostStepPoint()->GetPosition().z()/CLHEP::cm <<" cm" << G4endl;
	  }
          else{
  G4cout<< " x_pos: " << step->GetPostStepPoint()->GetPosition().x()/CLHEP::cm <<" cm" << G4endl;
  G4cout<< " y_pos: " << step->GetPostStepPoint()->GetPosition().y()/CLHEP::cm <<" cm" << G4endl;
  G4cout<< " z_pos: " << step->GetPostStepPoint()->GetPosition().z()/CLHEP::cm <<" cm" << G4endl;
  //抓取PKA能谱，飞行距离

  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->FillNtupleDColumn(1, step->GetPostStepPoint()->GetPosition().x());
  analysisManager->FillNtupleDColumn(2, step->GetPostStepPoint()->GetPosition().y());
  analysisManager->FillNtupleDColumn(3, step->GetPostStepPoint()->GetPosition().z());
  //抓取PKA能谱，飞行距离

  const G4Track* track = step->GetTrack();
  if (track->GetTrackID() > 1) {

    // 获取次级粒子的粒子定义
    const G4ParticleDefinition* particleDefinition = track->GetDefinition();

    // 判断是否为 12C
    if (particleDefinition->GetParticleName() == "C12") {
        // 获取次级粒子的能量和轨迹长度
        G4double secondaryEnergy = track->GetVertexKineticEnergy();
        G4double trackLength = track->GetTrackLength();
        G4int trackID = track->GetTrackID();

        auto analysisManager = G4AnalysisManager::Instance();
        
        // 填充 ROOT 直方图
        analysisManager->FillH1(1, secondaryEnergy);
        analysisManager->FillH1(2, trackLength);

        // 初始化 PKA/SKA 数据（默认为 0）
        G4double PKA_E = 0.0, SKA_E = 0.0;
        G4double PKA_length = 0.0, SKA_length = 0.0;

        // 判断是 PKA 还是 SKA
        if (trackID == 2) {  // PKA
            PKA_E = secondaryEnergy;
            PKA_length = trackLength;
        } else if (trackID > 2) {  // SKA
            SKA_E = secondaryEnergy;
            SKA_length = trackLength;
        }

        // 填充 Ntuple 数据
        G4cout << "DEBUG: PKA_E = " << PKA_E/ CLHEP::keV << " keV" << " | TrackID = " << trackID << G4endl;
        analysisManager->FillNtupleDColumn(4, PKA_E);
        //analysisManager->FillNtupleDColumn(4, 100);
        analysisManager->FillNtupleDColumn(5, SKA_E);
        analysisManager->FillNtupleDColumn(6, PKA_length);
        analysisManager->FillNtupleDColumn(7, SKA_length);
        analysisManager->FillNtupleIColumn(8, trackID);
        
        // 记录输出信息
        G4cout << "Secondary Particle Energy: " << secondaryEnergy / CLHEP::keV << " keV" << G4endl;
        G4cout << "Secondary Particle Type: " << particleDefinition->GetParticleName() << G4endl;
        G4cout << "Track ID: " << trackID << G4endl;
        
        
        analysisManager->AddNtupleRow();
        

        }
    }


  }  
  }	 

  // collect energy deposited in this step
  G4double edepStep = step->GetTotalEnergyDeposit();
//  G4cout << "edep:" << edepStep / CLHEP::MeV << "MeV" << G4endl;
  fEventAction->AddEdep(edepStep);

// Get position of step, code from Geant4 forum
// get the status of the pre step point
//G4StepStatus PreStepStatus = step->GetPreStepPoint()->GetStepStatus();
// get the volume of the post step point
//G4LogicalVolume* PostStepPoint = step->GetPostStepPoint()->GetTouchableHandle()->GetVolume()->GetLogicalVolume();
// Check if
///

//写入坐标
//  dataFile.open("edep.txt",std::ios::app|std::ios::out);
//  dataFile<< edepStep << G4endl;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
}
}
