#include "../../YTE/Physics/Transform.hpp"

#include "Translate.hpp"
#include "Scale.hpp"
#include "Rotate.hpp"
#include "Axis.hpp"


Axis::Axis(int aDir, YTE::Engine * aEngine, YTE::Space * aSpace) 
  : YTE::Composition(aEngine, aSpace), mDir(aDir)
{

}

void Axis::AddComponents()
{
  // add transform
  YTE::Transform *transform = new YTE::Transform(this, mSpace, nullptr);

  // add translate
  YTE::Translate *translate = new YTE::Translate(mDir, this, mSpace, nullptr);

  // add scale
  YTE::Scale *scale = new YTE::Scale(mDir, this, mSpace, nullptr);

  // add rotate
  YTE::Rotate *rotate = new YTE::Rotate(mDir, this, mSpace, nullptr);

  mTranslateModel = new YTE::Model(this, mSpace, nullptr);
  mScaleModel = new YTE::Model(this, mSpace, nullptr);
  mRotateModel = new YTE::Model(this, mSpace, nullptr);

  std::string trMesh = "Jimy.fbx";
  std::string scMesh = "Jimy.fbx";
  std::string rtMesh = "Jimy.fbx";
  
  switch (mDir)
  {
  case X:
  {
    trMesh = "Move_X.fbx";
    scMesh = "Scale_X.fbx";
    rtMesh = "Rotate_X.fbx";
    break;
  }

  case Y:
  {
    trMesh = "Move_Y.fbx";
    scMesh = "Scale_Y.fbx";
    rtMesh = "Rotate_Y.fbx";
    break;
  }

  case Z:
  {
    trMesh = "Move_Z.fbx";
    scMesh = "Scale_Z.fbx";
    rtMesh = "Rotate_Z.fbx";
    break;
  }
  }
    
  mTranslateModel->SetMesh(trMesh);
  mScaleModel->SetMesh(scMesh);
  mRotateModel->SetMesh(rtMesh);


  auto *comps = this->GetComponents();

  comps->Emplace(transform->GetType(), transform);
  comps->Emplace(mTranslateModel->GetType(), mTranslateModel);
  comps->Emplace(translate->GetType(), translate);
  comps->Emplace(scale->GetType(), scale);
  comps->Emplace(rotate->GetType(), rotate);
}