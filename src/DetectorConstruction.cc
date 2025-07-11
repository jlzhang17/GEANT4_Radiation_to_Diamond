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
/// \file DetectorConstruction.cc
/// \brief Implementation of the B1::DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

namespace B1
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  // Envelope parameters
  //
  G4double env_sizeXY = 20000*nm, env_sizeZ = 0.03*cm;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_Galactic");

  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;

  //
  // World
  //
  G4double world_sizeXY = env_sizeXY;
  G4double world_sizeZ  = env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");

  G4Box* solidWorld =
    new G4Box("World",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size

  G4LogicalVolume* logicWorld =
    new G4LogicalVolume(solidWorld,          //its solid
                        world_mat,           //its material
                        "World");            //its name

  G4VPhysicalVolume* physWorld =
    new G4PVPlacement(0,                     //no rotation
                      G4ThreeVector(),       //at (0,0,0)
                      logicWorld,            //its logical volume
                      "World",               //its name
                      0,                     //its mother  volume
                      false,                 //no boolean operation
                      0,                     //copy number
                      checkOverlaps);        //overlaps checking

  //
  // Envelope
  //
  G4Box* solidEnv =
    new G4Box("Envelope",                    //its name
        env_sizeXY, env_sizeXY, env_sizeZ); //its size, a,b,c是长宽高的一半

  G4LogicalVolume* logicEnv =
    new G4LogicalVolume(solidEnv,            //its solid
                        env_mat,             //its material
                        "Envelope");         //its name

  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(),         //at (0,0,0)
                    logicEnv,                //its logical volume
                    "Envelope",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking

  //
  // Shape 1
  //
 // G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_C");
 // G4ThreeVector pos1 = G4ThreeVector(0, 2*cm, 0*cm);

  // Conical section shape
 // G4double shape1_rmina =  0.*cm, shape1_rmaxa = 2.*cm;
 // G4double shape1_rminb =  0.*cm, shape1_rmaxb = 4.*cm;
 // G4double shape1_hz = 3.*cm;
 // G4double shape1_phimin = 0.*deg, shape1_phimax = 360.*deg;
 // G4Cons* solidShape1 =
 //   new G4Cons("Shape1",
 //   shape1_rmina, shape1_rmaxa, shape1_rminb, shape1_rmaxb, shape1_hz,
  //  shape1_phimin, shape1_phimax);

//  G4LogicalVolume* logicShape1 =
  //  new G4LogicalVolume(solidShape1,         //its solid
  //                      shape1_mat,          //its material
  //                      "Shape1");           //its name

 // new G4PVPlacement(0,                       //no rotation
   //                 pos1,                    //at position
   //                 logicShape1,             //its logical volume
   //                 "Shape1",                //its name
   //                 logicEnv,                //its mother  volume
   //                 false,                   //no boolean operation
   //                 0,                       //copy number
   //             checkOverlaps);          //overlaps checking
  //Diamond
 //Define diamond


G4double A = 12.01 * g/mole;
G4double Z = 6;
 G4Material* diamond = new G4Material("diamond", Z, A, 3.515*g/cm3);
  G4ThreeVector pos = G4ThreeVector(0, 0*cm, 0.015*cm);
  
  G4double diamond_sizeXY = 200*nm, diamond_sizeZ = 0.015*cm;
 
  G4Box* solidShape = new G4Box("Shape", diamond_sizeXY, diamond_sizeXY, diamond_sizeZ);

  G4LogicalVolume* logicShape =
    new G4LogicalVolume(solidShape,         //its solid
                       diamond, 
                        "Shape");           //its name
    
  new G4PVPlacement(0,                       //no rotation
                    pos,                    //at position
                    logicShape,             //its logical volume
                    "Shape",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking


  //
  // Shape 2
  //    

  // Set Shape2 as scoring volume
  //
  fScoringVolume = logicShape;

  //
  //always return the physical World
  //
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
