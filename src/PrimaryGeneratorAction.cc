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
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the B1::PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "DetectorConstruction.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
{
  G4int n_particle = 1;
  fParticleGun  = new G4ParticleGun(n_particle);

  // default particle kinematic
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
    = particleTable->FindParticle(particleName="proton");
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
  fParticleGun->SetParticleEnergy(24000.*MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
}

//fast neutron energy spectrum
G4double PrimaryGeneratorAction::GetEnergyFromSpectrum()
{
    G4double E;
    G4double probability;
    G4double maxProbability = 1.0;  // 需要根据你的能谱函数计算出实际的最大值

    while (true)
    {
        E = G4UniformRand() * (7 - 0.000001) + 0.000001;  // 随机生成能量
        probability = (0.470 * std::exp(-0.693 * E) + 0.39 * std::exp(-0.97 * E) / std::pow(E, 0.88)) * 1e12;

        if (G4UniformRand() * maxProbability < probability)
        {
            return E * MeV;
        }
    }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of ecah event
  //

  // In order to avoid dependence of PrimaryGeneratorAction
  // on DetectorConstruction class we get Envelope volume
  // from G4LogicalVolumeStore.
    // 获取从能谱分布中抽取的能量

  // G4double energy = fParticleGun->GetParticleEnergy();
  // G4cout << "Debug: Current particle energy = " << energy / MeV << " MeV" << G4endl;
  //   // 设置粒子枪的能量
  // fParticleGun->SetParticleEnergy(energy);
  // fPrimaryEnergies.push_back(energy); // 
  G4double energyBefore = fParticleGun->GetParticleEnergy();
  fParticleGun->GeneratePrimaryVertex(anEvent);
  G4double energyAfter = fParticleGun->GetParticleEnergy();
  fPrimaryEnergies.push_back(fParticleGun->GetParticleEnergy()); // 仅记录

  G4cout << "Energy before generation: " << energyBefore / MeV << " MeV" << G4endl;
  G4cout << "Energy after generation: " << energyAfter / MeV << " MeV" << G4endl;

  if (fPrimaryParticleName.empty()) {
    fPrimaryParticleName = fParticleGun->GetParticleDefinition()->GetParticleName(); // 
  }
  G4double envSizeXY = 0;
  G4double envSizeZ = 0;

  if (!fEnvelopeBox)
  {
    G4LogicalVolume* envLV
      = G4LogicalVolumeStore::GetInstance()->GetVolume("Envelope");
    if ( envLV ) fEnvelopeBox = dynamic_cast<G4Box*>(envLV->GetSolid());
  }

  if ( fEnvelopeBox ) {
    envSizeXY = fEnvelopeBox->GetXHalfLength();
    envSizeZ = fEnvelopeBox->GetZHalfLength();
  }
  else  {
    G4ExceptionDescription msg;
    msg << "Envelope volume of box shape not found.\n";
    msg << "Perhaps you have changed geometry.\n";
    msg << "The gun will be place at the center.";
    G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
     "MyCode0002",JustWarning,msg);
  }

  G4double size = 0.005;
  G4double x0 = size * envSizeXY * (G4UniformRand()-0.5);
  G4double y0 = size * envSizeXY * (G4UniformRand()-0.5);
  G4double z0 = 0 * envSizeZ;
  //G4cout << "particle_gun_position"<<"|" << envSizeXY/CLHEP::nm <<"|" <<envSizeXY/CLHEP::nm <<"|" << envSizeZ/CLHEP::nm << G4endl;
  fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
  G4cout << "particle_gun_position" << x0/CLHEP::nm <<"|" <<y0/CLHEP::nm<<"|" << z0/CLHEP::nm << G4endl;



  fParticleGun->GeneratePrimaryVertex(anEvent);
}
  G4double PrimaryGeneratorAction::GetPrimaryEnergy() const {
  if (!fPrimaryEnergies.empty()) {
    return fPrimaryEnergies.front(); // 取第一个能量，也可以用.back()取最后一个
  }
  return 0.;
}           
  G4String PrimaryGeneratorAction::GetPrimaryParticleName() const {
  return fPrimaryParticleName;
}

std::vector<G4double> PrimaryGeneratorAction::GetPrimaryEnergies() const {
  return fPrimaryEnergies;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}

