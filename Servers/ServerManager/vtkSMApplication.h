/*=========================================================================

  Program:   ParaView
  Module:    vtkSMApplication.h

  Copyright (c) Kitware, Inc.
  All rights reserved.
  See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSMApplication - provides initialization and finalization for server manager
// .SECTION Description
// vtkSMApplication provides methods to initialize and finalize the
// server manager.

#ifndef __vtkSMApplication_h
#define __vtkSMApplication_h

#include "vtkSMObject.h"

//BTX
struct vtkSMApplicationInternals;
//ETX
class vtkSMPluginManager;
class vtkQtInitialization;

class VTK_EXPORT vtkSMApplication : public vtkSMObject
{
public:
  static vtkSMApplication* New();
  vtkTypeMacro(vtkSMApplication, vtkSMObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Perform initialization: add the server manager symbols to the
  // interpreter, read default interfaces from strings, create
  // singletons... Should be called before any server manager objects
  // are created.
  void Initialize();

  // Description:
  // Cleanup: cleans singletons
  // Should be called before exit, after all server manager objects
  // are deleted.
  void Finalize();

  // Description:
  int ParseConfigurationFile(const char* fname, const char* dir);

  // Description:
  int ParseConfiguration(const char* configuration);

  // Description:
  void AddConfigurationFile(const char* fname, const char* dir);

  // Description:
  unsigned int GetNumberOfConfigurationFiles();

  // Description:
  void GetConfigurationFile(
    unsigned int idx, const char*& fname, const char*& dir);

  // Description:
  void ParseConfigurationFiles();
  
  // Description:
  // Get the root for Settings, given the server connection ID.
  const char* GetSettingsRoot(vtkIdType connectionId);    
 
  // Description:
  // Plugin manager managing all server manager plugins.
  vtkSMPluginManager* GetPluginManager();

  // Description:
  // Certain server manager components need Qt. Those need to verify that the
  // QApplication has been initialized. They can use this method to initialize
  // the QApplication if it hasn't been initialized. This has any effect only
  // when compiled with VTK_USE_QVTK.
  void EnsureQApplicationIsInitialized();

protected:
  vtkSMApplication();
  ~vtkSMApplication();

  vtkSMApplicationInternals* Internals;
  vtkQtInitialization* QtInitializer;
private:
  vtkSMApplication(const vtkSMApplication&); // Not implemented
  void operator=(const vtkSMApplication&); // Not implemented
};

#endif
