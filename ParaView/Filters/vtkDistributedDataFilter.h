/*=========================================================================

  Program:   ParaView
  Module:    vtkDistributedDataFilter.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*----------------------------------------------------------------------------
 Copyright (c) Sandia Corporation
 See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.
----------------------------------------------------------------------------*/

// .NAME vtkDistributedDataFilter
//
// .SECTION Description
//    This filter redistributes data among processors in a parallel
//    application into spatially contiguous vtkUnstructuredGrids.
//    The execution model anticipated is that all processes read in
//    part of a large vtkDataSet.  Each process sets the input of
//    filter to be that DataSet.  When executed, this filter builds
//    in parallel a k-d tree decomposing the space occupied by the
//    distributed DataSet into spatial regions.  It assigns each
//    spatial region to a processor.  The data is then redistributed
//    and the output is a single vtkUnstructuredGrid containing the
//    cells in the process' assigned regions.
//
// .SECTION Caveats
//    The Execute() method must be called by all processes in the
//    parallel application, or it will hang.  If you are not certain
//    that your pipelines will execute identically on all processors,
//    you may want to use this filter in an explicit execution mode.
//
// .SECTION See Also
//      vtkKdTree vtkPKdTree

#ifndef __vtkDistributedDataFilter_h
#define __vtkDistributedDataFilter_h

#include <vtkDataSetToUnstructuredGridFilter.h>

class vtkUnstructuredGrid;
class vtkPKdTree;
class vtkMultiProcessController;
class vtkTimerLog;
class vtkDataArray;
class vtkIntArray;
class vtkIdList;
class vtkUnstructuredGrid;
#include <map>

class VTK_EXPORT vtkDistributedDataFilter: public vtkDataSetToUnstructuredGridFilter
{
  vtkTypeRevisionMacro(vtkDistributedDataFilter, 
    vtkDataSetToUnstructuredGridFilter);

public:
  void PrintSelf(ostream& os, vtkIndent indent);
  void PrintTiming(ostream& os, vtkIndent indent);

  static vtkDistributedDataFilter *New();

  // Description:
  //   Set/Get the communicator object

  void SetController(vtkMultiProcessController *c);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);

  // Description:
  //    It is desirable to have a field array of global node IDs
  //    for two reasons:
  //
  //    1. When merging together sub grids that were distributed
  //    across processors, global node IDs can be used to remove
  //    duplicate points and significantly reduce the size of the
  //    resulting output grid.  If no such array is available,
  //    D3 will use a tolerance to merge points, which is much
  //    slower.
  //
  //    2. If ghost cells have been requested, D3 requires a
  //    global node ID array in order to request and transfer
  //    ghost cells in parallel among the processors.  If there
  //    is no global node ID array, D3 will in parallel create
  //    a global node ID array, and the time to do this can be
  //    significant.
  //    
  //    If you know the name of a global node ID array in the input
  //    dataset, set that name with this method.  If you leave
  //    it unset, D3 will search the input data set for certain
  //    common names of global node ID arrays.  If none is found,
  //    and ghost cells have been requested, D3 will create a
  //    temporary global node ID array before aquiring ghost cells.
  //
  //    If you set the name of the global node ID array to be a
  //    name which does not exist in the input data set, D3 will
  //    create an array of that name in the output and populate
  //    it with a global node ID array.  Note that this is done
  //    before clipping (if ClipCells is ON).  Since ClipCells
  //    tetrahedralizes it's input and interpolates it's field
  //    values, the global node IDs become meaningless.  Hence,
  //    whether original or generated by D3, the global node ID
  //    array, if any, is deleted from the output unstructured grid
  //    if clipping occured.

  vtkSetStringMacro(GlobalIdArrayName);
  vtkGetStringMacro(GlobalIdArrayName);
  
  // Description:
  //    When this filter executes, it creates a vtkPKdTree (K-d tree)
  //    data structure in parallel which divides the total distributed 
  //    data set into spatial regions.  The K-d tree object also creates 
  //    tables describing which processes have data for which 
  //    regions.  Only then does this filter redistribute 
  //    the data according to the region assignment scheme.  By default, 
  //    the K-d tree structure and it's associated tables are deleted
  //    after the filter executes.  If you anticipate changing only the
  //    region assignment scheme (input is unchanged) and explicitly
  //    re-executing, then RetainKdTreeOn, and the K-d tree structure and
  //    tables will be saved.  Then, when you re-execute, this filter will
  //    skip the k-d tree build phase and go straight to redistributing
  //    the data according to region assignment.  See vtkPKdTree for
  //    more information about region assignment.

  vtkBooleanMacro(RetainKdtree, int);
  vtkGetMacro(RetainKdtree, int);
  vtkSetMacro(RetainKdtree, int);

  // Description:
  //   Get a pointer to the parallel k-d tree object.  Required for changing
  //   default behavior for region assignment, changing default depth of tree,
  //   or other tree building default parameters.  See vtkPKdTree and 
  //   vtkKdTree for more information about these options.

  vtkPKdTree *GetKdtree();

  // Description:
  //   Each cell in the data set is associated with one of the
  //   spatial regions of the k-d tree decomposition.  In particular,
  //   the cell belongs to the region that it's centroid lies in.
  //   When the new vtkUnstructuredGrid is created, by default it
  //   is composed of the cells associated with the region(s)
  //   assigned to this process.  If you also want it to contain
  //   cells that intersect these regions, but have their centroid
  //   elsewhere, then set this variable on.  By default it is off.

  vtkBooleanMacro(IncludeAllIntersectingCells, int);
  vtkGetMacro(IncludeAllIntersectingCells, int);
  vtkSetMacro(IncludeAllIntersectingCells, int);

  // Description:
  //   Set this variable if you want the cells of the output
  //   vtkUnstructuredGrid to be clipped to the spatial region
  //   boundaries.  By default this is off.

  vtkBooleanMacro(ClipCells, int);
  vtkGetMacro(ClipCells, int);
  vtkSetMacro(ClipCells, int);

  // Description:
  //   Build a vtkUnstructuredGrid for a spatial region from the 
  //   data distributed across processes.  Execute() must be called
  //   by all processes, or it will hang.

  void Execute();
  void SingleProcessExecute();
  void ExecuteInformation();


  // Description:
  //  Turn on collection of timing data

  vtkBooleanMacro(Timing, int);
  vtkSetMacro(Timing, int);
  vtkGetMacro(Timing, int);

  // Description:
  // Consider the MTime of the KdTree.
  unsigned long GetMTime();

protected:

  vtkDistributedDataFilter();
  ~vtkDistributedDataFilter();

private:

  static const char *TemporaryGlobalCellIds;
  static const char *TemporaryInsideBoxFlag;
  static const char *TemporaryGlobalNodeIds;
  static const unsigned char UnsetGhostLevel; 

  int CheckFieldArrayTypes(vtkDataSet *set);

  void ComputeFanIn(int *member, int nParticipants, int myLocalRank,
    int **source, int *nsources, int *target, int *ntargets);

  vtkUnstructuredGrid *ExtractCellsForProcess(int proc, vtkDataSet *in);
  vtkUnstructuredGrid *ExtractTwoGhostLevels(int proc, vtkDataSet *in);
  vtkUnstructuredGrid *ExtractOneGhostLevel( vtkIntArray *regions, vtkDataSet *in);

  vtkUnstructuredGrid *MPIRedistribute(vtkDataSet *in);
  int PairWiseDataExchange(int *yourSize, char **yourData, int tag);

  char *MarshallDataSet(vtkUnstructuredGrid *extractedGrid, int &size);
  vtkUnstructuredGrid *UnMarshallDataSet(char *buf, int size);

  vtkUnstructuredGrid *GenericRedistribute(vtkDataSet *in);

  vtkUnstructuredGrid *ReduceUgridMerge(vtkUnstructuredGrid *ugrid, int root);

  void ClipCellsToSpatialRegion(vtkUnstructuredGrid *grid);

  void ClipWithVtkClipDataSet(vtkUnstructuredGrid *grid, double *bounds,
           vtkUnstructuredGrid **outside, vtkUnstructuredGrid **inside);
  void ClipWithBoxClipDataSet(vtkUnstructuredGrid *grid, double *bounds,
           vtkUnstructuredGrid **outside, vtkUnstructuredGrid **inside);


  const char *GetGlobalNodeIdArray(vtkDataSet *set);
  int GlobalNodeIdAccessGetId(int idx);
  int GlobalNodeIdAccessStart(vtkDataSet *set);

  void AssignGlobalNodeIds(vtkUnstructuredGrid *grid);
  vtkIntArray *AssignGlobalCellIds();

  void FindGlobalPointIds(int **idarray, int *count, float **ptarray,
                          vtkIntArray *ids, vtkUnstructuredGrid *grid);

  int InMySpatialRegion(float x, float y, float z);
  int InMySpatialRegion(double x, double y, double z);
  int StrictlyInsideMyBounds(float x, float y, float z);
  int StrictlyInsideMyBounds(double x, double y, double z);

  vtkUnstructuredGrid *AddGhostCells(vtkUnstructuredGrid *grid);

//BTX
  vtkUnstructuredGrid *AddGhostLevel( vtkUnstructuredGrid *myGrid,
     vtkUnstructuredGrid *ghostPointGrid, int firstGhostPoint,
     vtkUnstructuredGrid *ghostCellGrid, 
     vtkstd::map<int, int> *globalToLocalMap,
     int ghostLevel);
//ETX

  void FixGhostLevels(vtkUnstructuredGrid *grid);
  vtkIntArray **GetGhostPoints(int ghostLevel, vtkUnstructuredGrid *grid,
                            int startPointId);
  vtkIntArray **ExchangePointIds(vtkIntArray **myPtIds);
//BTX
  vtkUnstructuredGrid **BuildRequestedGrids( vtkIntArray **globalPtIds,
                        vtkUnstructuredGrid *grid,
                        vtkstd::map<int, int> *ptIdMap);
  vtkUnstructuredGrid *ExchangeMergeSubGrids(vtkUnstructuredGrid **grids, 
          vtkUnstructuredGrid *ghostCellGrid,
          int ghostLevel, vtkstd::map<int, int> *idMap);
//ETX

  vtkDataArray *FindGlobalNodeIdArrayInDataSet(vtkDataSet *set);

  static void AddConstantUnsignedCharPointArray(vtkUnstructuredGrid *grid, 
                                 const char *arrayName, unsigned char val);
  static void AddConstantUnsignedCharCellArray(vtkUnstructuredGrid *grid, 
                                 const char *arrayName, unsigned char val);
  static void FreeIdArrays(vtkIntArray **idArray, int n);
  void RemoveRemoteCellsFromList(vtkIdList *cellList, vtkIntArray *gidCells, 
                                 int *remoteCells, int nRemoteCells);





  vtkPKdTree *Kdtree;
  vtkMultiProcessController *Controller;

  int NumProcesses;
  int MyId;

  int NumConvexSubRegions;
  double *ConvexSubRegionBounds;

  int GhostLevel;

  char *GlobalIdArrayName;    // global node IDs supplied by user

  char *GlobalIdArrayChar;
  short *GlobalIdArrayShort;
  int *GlobalIdArrayInt;
  long *GlobalIdArrayLong;
  vtkIdType *GlobalIdArrayIdType;

  int RetainKdtree;
  int IncludeAllIntersectingCells;
  int ClipCells;

  int Timing;

  vtkTimerLog *TimerLog;

  vtkDistributedDataFilter(const vtkDistributedDataFilter&); // Not implemented
  void operator=(const vtkDistributedDataFilter&); // Not implemented
};
#endif
