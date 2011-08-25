/*=========================================================================
 *
 *  Copyright David Doria 2011 daviddoria@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef FORM_H
#define FORM_H

#include "ui_Form.h"

// VTK
#include <vtkSmartPointer.h>
#include <vtkSeedWidget.h>
#include <vtkPointHandleRepresentation2D.h>

// ITK
#include "itkImage.h"

// Qt
#include <QMainWindow>

// Custom
#include "Types.h"
#include "PointSelectionStyle2D.h"

// Forward declarations
class vtkRenderer;
class vtkImageData;
class vtkImageActor;
class vtkActor;

class Form : public QMainWindow, public Ui::Form
{
  Q_OBJECT
public:

  // Constructor/Destructor
  Form();
  ~Form() {};

public slots:
  void on_actionOpenImage1_activated();
  void on_actionOpenImage2_activated();
  void on_actionSaveImage1Points_activated();
  void on_actionSaveImage2Points_activated();

  void on_btnDeleteAllImage1_clicked();
  void on_btnDeleteAllImage2_clicked();
  void on_btnDeleteLastImage1_clicked();
  void on_btnDeleteLastImage2_clicked();

  void on_actionHelp_activated();
  
protected:

  // Image1
  FloatVectorImageType::Pointer Image1;
  vtkSmartPointer<vtkImageActor> Image1Actor;
  vtkSmartPointer<vtkImageData> Image1Data;
  vtkSmartPointer<PointSelectionStyle2D> Image1SelectionStyle2D;
  vtkSmartPointer<vtkRenderer> LeftRenderer;
  
  // Image2
  FloatVectorImageType::Pointer Image2;
  vtkSmartPointer<vtkImageActor> Image2Actor;
  vtkSmartPointer<vtkImageData> Image2Data;
  vtkSmartPointer<PointSelectionStyle2D> Image2SelectionStyle2D;
  vtkSmartPointer<vtkRenderer> RightRenderer;
  
};

#endif // Form_H
