/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAMRDualContour.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkAMRDualContour - Extract particles and analyse them.
// .SECTION Description
// This filter takes a cell data volume fraction and generates a polydata
// surface.  It also performs connectivity on the particles and generates
// a particle index as part of the cell data of the output.  It computes
// the volume of each particle from the volume fraction.

// This will turn on validation and debug i/o of the filter.
//#define vtkAMRDualContourDEBUG
#define vtkAMRDualContourPROFILE

#ifndef __vtkAMRDualContour_h
#define __vtkAMRDualContour_h

#include "vtkMultiBlockDataSetAlgorithm.h"
#include "vtkstd/vector"//
#include "vtkstd/string"//

#include "vtkSmartPointer.h"
#include "vtkTimerLog.h"

class vtkDataSet;
class vtkImageData;
class vtkPolyData;
class vtkHierarchicalBoxDataSet;
class vtkPoints;
class vtkDoubleArray;
class vtkCellArray;
class vtkCellData;
class vtkIntArray;
class vtkMultiProcessController;
class vtkDataArraySelection;
class vtkCallbackCommand;

class vtkAMRDualGridHelper;
class vtkAMRDualGridHelperBlock;
class vtkAMRDualGridHelperFace;


class VTK_EXPORT vtkAMRDualContour : public vtkMultiBlockDataSetAlgorithm
{
public:
  static vtkAMRDualContour *New();
  vtkTypeRevisionMacro(vtkAMRDualContour,vtkMultiBlockDataSetAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkSetMacro(IsoValue, double);
  vtkGetMacro(IsoValue, double);

  // Description:
  // These are to evaluate performances. You can turn off capping, degenerate cells
  // and multiprocess comunication to see how they affect speed of execution.
  // Degenerate cells is the meshing between levels in the grid.
  vtkSetMacro(EnableCapping,int);
  vtkGetMacro(EnableCapping,int);
  vtkBooleanMacro(EnableCapping,int);
  vtkSetMacro(EnableDegenerateCells,int);
  vtkGetMacro(EnableDegenerateCells,int);
  vtkBooleanMacro(EnableDegenerateCells,int);
  vtkSetMacro(EnableMultiProcessCommunication,int);
  vtkGetMacro(EnableMultiProcessCommunication,int);
  vtkBooleanMacro(EnableMultiProcessCommunication,int);

protected:
  vtkAMRDualContour();
  ~vtkAMRDualContour();

  double IsoValue;

  // Algorithm options that may improve performance.
  int EnableDegenerateCells;
  int EnableCapping;
  int EnableMultiProcessCommunication;

  //BTX
  virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  virtual int FillInputPortInformation(int port, vtkInformation *info);
  virtual int FillOutputPortInformation(int port, vtkInformation *info);

  void ProcessBlock(vtkAMRDualGridHelperBlock* block, int blockId);
  
  void ProcessDualCell(
    vtkAMRDualGridHelperBlock* block, int blockId,
    int marchingCase,
    int x, int y, int z,
    double values[8]);

  void CapCell(
    // Which cell faces need to be capped.
    unsigned char cubeBoundaryBits,
    // Marching cubes case for this cell
    int cubeCase,
    // Ids of the point created on edges for the internal surface
    vtkIdType edgePtIds[12],
    // Locations of 8 corners. (xyz4xyz4...) 4th value is not used.
    double cornerPoints[32],
    // For block id array (for debugging).  I should just make this an ivar.
    int blockId);

  // Stuff exclusively for debugging.
  vtkIntArray* BlockIdCellArray;

  // Ivars used to reduce method parrameters.
  vtkAMRDualGridHelper* Helper;
  vtkPoints* Points;
  vtkCellArray* Faces;

  vtkMultiProcessController *Controller;

  // I made these ivars to avoid allocating multiple times.
  // The buffer is not used too many times, but .....
  int* MessageBuffer;
  int* MessageBufferLength;

private:
  vtkAMRDualContour(const vtkAMRDualContour&);  // Not implemented.
  void operator=(const vtkAMRDualContour&);  // Not implemented.
  //ETX
};

#endif