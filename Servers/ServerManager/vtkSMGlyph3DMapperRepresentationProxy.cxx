/*=========================================================================

  Program:   ParaView
  Module:    vtkSMGlyph3DMapperRepresentationProxy.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSMGlyph3DMapperRepresentationProxy.h"

#include "vtkObjectFactory.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMSourceProxy.h"
#include "vtkProcessModule.h"
#include "vtkSMRepresentationStrategy.h"
#include "vtkSmartPointer.h"
#include "vtkSMViewProxy.h"

vtkStandardNewMacro(vtkSMGlyph3DMapperRepresentationProxy);
vtkCxxRevisionMacro(vtkSMGlyph3DMapperRepresentationProxy, "$Revision: 1774 $");
//----------------------------------------------------------------------------
vtkSMGlyph3DMapperRepresentationProxy::vtkSMGlyph3DMapperRepresentationProxy()
{
  this->GlyphMapper = 0;
  this->SourceOutputPort = 0;
}

//----------------------------------------------------------------------------
vtkSMGlyph3DMapperRepresentationProxy::~vtkSMGlyph3DMapperRepresentationProxy()
{
}

//----------------------------------------------------------------------------
void vtkSMGlyph3DMapperRepresentationProxy::AddInput(
  unsigned int inputPort, vtkSMSourceProxy* input,
  unsigned int outputPort, const char* method)
{
  if (inputPort == 0)
    {
    this->Superclass::AddInput(inputPort, input, outputPort, method);
    return;
    }

  if (!input)
    {
    vtkErrorMacro("Source input cannot be NULL.");
    return;
    }

  input->CreateOutputPorts();
  int numPorts = input->GetNumberOfOutputPorts();

  if (numPorts <= 0 || static_cast<unsigned int>(numPorts) <= outputPort)
    {
    vtkErrorMacro("Input has no output (or required output port). "
      "Cannot create the representation.");
    return;
    }

  this->Source = input;
  this->SourceOutputPort = outputPort;
}

//----------------------------------------------------------------------------
bool vtkSMGlyph3DMapperRepresentationProxy::BeginCreateVTKObjects()
{
  if (!this->Superclass::BeginCreateVTKObjects())
    {
    return false;
    }

  this->GlyphMapper = this->GetSubProxy("GlyphMapper");
  this->GlyphMapper->SetServers(
    vtkProcessModule::CLIENT|vtkProcessModule::RENDER_SERVER);
  return true;
}

//----------------------------------------------------------------------------
bool vtkSMGlyph3DMapperRepresentationProxy::EndCreateVTKObjects()
{
  if (!this->Superclass::EndCreateVTKObjects())
    {
    return false;
    }

  // Switch the mapper to use the GlyphMapper instead of the standard mapper.
  this->Connect(this->GlyphMapper, this->Prop3D, "Mapper");
  this->GlyphMapper->UpdateVTKObjects();
  return true;
}

//----------------------------------------------------------------------------
bool vtkSMGlyph3DMapperRepresentationProxy::InitializeStrategy(vtkSMViewProxy* view)
{
  if (!this->Superclass::InitializeStrategy(view))
    {
    return false;
    }

  vtkSMPropertyHelper(this->GlyphMapper, "Input").Set(
    vtkSMPropertyHelper(this->Mapper, "Input").GetAsProxy());

  // source input is always polydata.
  vtkSmartPointer<vtkSMRepresentationStrategy> strategy;
  strategy.TakeReference(view->NewStrategy(VTK_POLY_DATA));
  if (!strategy.GetPointer())
    {
    vtkErrorMacro("View could not provide a strategy to use. "
      << "Cannot be rendered in this view of type " << view->GetClassName());
    return false;
    }

  // Now initialize the data pipelines involving this strategy.
  // Since representations are not added to views unless their input is set, we
  // can assume that the objects for this proxy have been created.
  // (Look at vtkSMDataRepresentationProxy::AddToView()).

  strategy->SetEnableLOD(false);

  this->Connect(this->Source, strategy, "Input", this->SourceOutputPort);
  this->Connect(strategy->GetOutput(), this->GlyphMapper, "Source");

  // Creates the strategy objects.
  strategy->UpdateVTKObjects();

  this->AddStrategy(strategy);

  this->GlyphMapper->UpdateVTKObjects();
  return true;
}

//----------------------------------------------------------------------------
bool vtkSMGlyph3DMapperRepresentationProxy::GetBounds(double bounds[6])
{
  //this->Superclass::GetBounds(bounds);
  this->GlyphMapper->UpdatePropertyInformation();
  vtkSMPropertyHelper(this->GlyphMapper, "BoundsInfo").Get(bounds, 6);
  //cout << "Bounds: " << bounds[0] << ", " << bounds[1] << ", " << bounds[2]
  //  << bounds[3] << ", " << bounds[4] << ", " << bounds[5] << endl;
  return true;
}

//----------------------------------------------------------------------------
void vtkSMGlyph3DMapperRepresentationProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}


