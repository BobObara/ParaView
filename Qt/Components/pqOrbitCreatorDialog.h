/*=========================================================================

   Program: ParaView
   Module:    pqOrbitCreatorDialog.h

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 
   
   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

========================================================================*/
#ifndef __pqOrbitCreatorDialog_h 
#define __pqOrbitCreatorDialog_h

#include <QDialog>
#include <QList>
#include <QVariant>
#include "pqComponentsExport.h"

/// pqOrbitCreatorDialog is used by pqAnimationViewWidget to request the orbit
/// parameters from the user when the user want to create a camera animation track
/// that orbits some object(s). It's a simple dialog with a bunch of entries for
/// normal/center/radius of the orbit.
class PQCOMPONENTS_EXPORT pqOrbitCreatorDialog : public QDialog
{
  Q_OBJECT
  typedef QDialog Superclass;
public:
  pqOrbitCreatorDialog(QWidget* parent=0);
  virtual ~pqOrbitCreatorDialog();

  /// Returns the points the orbit based on the user chosen options.
  QList<QVariant> orbitPoints(int resolution) const;

  /// Returns the center for the orbit.
  QList<QVariant> center() const;

  void setNormal(double xyz[3]);

protected slots:
  void resetBounds();

private:
  pqOrbitCreatorDialog(const pqOrbitCreatorDialog&); // Not implemented.
  void operator=(const pqOrbitCreatorDialog&); // Not implemented.

  class pqInternals;
  pqInternals* Internals;
};

#endif


