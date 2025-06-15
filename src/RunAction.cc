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
/// \file RunAction.cc
/// \brief Implementation of the B1::RunAction class

#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
// #include "Run.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
//#include "Analysis.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
{
  // add new units for dose
  //
  const G4double milligray = 1.e-3*gray;
  const G4double microgray = 1.e-6*gray;
  const G4double nanogray  = 1.e-9*gray;
  const G4double picogray  = 1.e-12*gray;

  new G4UnitDefinition("milligray", "milliGy" , "Dose", milligray);
  new G4UnitDefinition("microgray", "microGy" , "Dose", microgray);
  new G4UnitDefinition("nanogray" , "nanoGy"  , "Dose", nanogray);
  new G4UnitDefinition("picogray" , "picoGy"  , "Dose", picogray);

  // Register accumulable to the accumulable manager
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->RegisterAccumulable(fEdep);
  accumulableManager->RegisterAccumulable(fEdep2);

  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetVerboseLevel(2);
  analysisManager->SetNtupleMerging(true);

  analysisManager->CreateH1("fEdep","Edep",200,0.,5*MeV);
  analysisManager->CreateH1("PKA_E","PKA_E",200,0.,300*keV);
  analysisManager->CreateH1("SKA_E","SKA_E",200,0.,300*keV);
  analysisManager->CreateH1("PKA_length","PKA_length",200,0.,400*nm);
  analysisManager->CreateH1("SKA_length","SKA_length",200,0.,400*nm);

  //创造一个一维直方图，名称维fEdep，标题维Edep，200个bins，横坐标范围0，10，单位是MeV
  analysisManager->CreateNtuple("Mydata","Energy deposit");
  analysisManager->CreateNtupleDColumn("fEdep");
  analysisManager->CreateNtupleDColumn("x_pos");
  analysisManager->CreateNtupleDColumn("y_pos");
  analysisManager->CreateNtupleDColumn("z_pos");
  analysisManager->CreateNtupleDColumn("PKA_E");
  analysisManager->CreateNtupleDColumn("SKA_E");
  analysisManager->CreateNtupleDColumn("PKA_length");
  analysisManager->CreateNtupleDColumn("SKA_length");
  analysisManager->CreateNtupleIColumn("TrackID");

  analysisManager->FinishNtuple();
  //"Mydata"：Ntuple 的名称，用于在输出文件中标识和检索 Ntuple,"Energy deposit"：Ntuple 的标题，用于描述 Ntuple 的内容或目的
  //
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*)
{
  // inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // reset accumulables to their initial values
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Reset();

  auto analysisManager = G4AnalysisManager::Instance();
 // analysisManager->SetDefaultFileType("root");
  G4String fileName = "Mydata.root";
// G4String fileName_1 = "Step_position";
  analysisManager->OpenFile(fileName);
  G4cout << "Using" << analysisManager->GetType() << G4endl;

//  analysisManager->OpenFile(fileName_1);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RunAction::SetPrimaryGenerator(const B1::PrimaryGeneratorAction* gen) {
  fPrimaryGenerator = gen;
}
void RunAction::SetPhysicsListName(const G4String& name) {
  fPhysicsListName = name;
}
void RunAction::EndOfRunAction(const G4Run* run)
{
  G4int nofEvents = run->GetNumberOfEvent();
  if (nofEvents == 0) return;
  
   auto analysisManager = G4AnalysisManager::Instance();
  // Merge accumulables
  G4AccumulableManager* accumulableManager = G4AccumulableManager::Instance();
  accumulableManager->Merge();

  // Compute dose = total energy deposit in a run and its variance
  //
  G4double edep  = fEdep.GetValue();
  G4double edep2 = fEdep2.GetValue();

  G4double rms = edep2 - edep*edep/nofEvents;
  if (rms > 0.) rms = std::sqrt(rms); else rms = 0.;

  const DetectorConstruction* detConstruction
   = static_cast<const DetectorConstruction*>
     (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
  G4double mass = detConstruction->GetScoringVolume()->GetMass();
  G4double dose = edep/mass;
  G4double rmsDose = rms/mass;

  // Run conditions
  //  note: There is no primary generator action object for "master"
  //        run manager for multi-threaded mode.
  const PrimaryGeneratorAction* generatorAction
   = static_cast<const PrimaryGeneratorAction*>
     (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  G4String runCondition;
  if (generatorAction)
  {
    const G4ParticleGun* particleGun = generatorAction->GetParticleGun();
    runCondition += particleGun->GetParticleDefinition()->GetParticleName();
    runCondition += " of ";
    G4double particleEnergy = particleGun->GetParticleEnergy();
    runCondition += G4BestUnit(particleEnergy,"Energy");
  }

  // Print
  //
  if (IsMaster()) {
    G4cout
     << G4endl
     << "--------------------End of Global Run-----------------------";
  }
  else {
    G4cout
     << G4endl
     << "--------------------End of Local Run------------------------";
  }

  G4cout
     << G4endl
     << " The run consists of " << nofEvents << " "<< runCondition
     << G4endl
     << " Cumulated dose per run, in scoring volume : "
     << G4BestUnit(dose,"Dose") << " rms = " << G4BestUnit(rmsDose,"Dose")
     << G4endl
     << "------------------------------------------------------------"
     << G4endl
     << G4endl;
  G4cout << "### ===== Simulation Summary =====" << G4endl;

  if (fPrimaryGenerator) {
    G4cout << "Primary Particle: " << fPrimaryGenerator->GetPrimaryParticleName() << G4endl;
//    G4cout << "Primary Energy: " << fPrimaryGenerator->GetPrimaryEnergy() / MeV << " MeV" << G4endl;
    std::vector<G4double> energies = fPrimaryGenerator->GetPrimaryEnergies();
    for (size_t i = 0; i < 3 && i < energies.size(); ++i) {
      G4cout << "Primary " << i + 1 << " Energy: " << energies[i] / MeV << " MeV" << G4endl;
    }
    G4cout << "Total Primary Particles: " << run->GetNumberOfEvent() << G4endl;
    G4cout << "Physics List: " << fPhysicsListName << G4endl;
  }

 // G4cout << "Physics List: QGSP_INCLXX_HP" << G4endl;  // 你如果有多个可以后期替换为动态方式
  G4cout << "### ==================================" << G4endl;

  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::AddEdep(G4double edep)
{

  fEdep  += edep;
  fEdep2 += edep*edep;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
