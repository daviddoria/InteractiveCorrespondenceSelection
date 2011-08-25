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

#include "ui_Form.h"
#include "Form.h"

// ITK
#include "itkCastImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkVector.h"

// Qt
#include <QFileDialog>
#include <QIcon>

// VTK
#include <vtkActor.h>
#include <vtkCommand.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleImage.h>
#include <vtkMath.h>
#include <vtkPointData.h>
#include <vtkPointPicker.h>
#include <vtkProperty2D.h>
#include <vtkPolyDataMapper.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSeedRepresentation.h>
#include <vtkSeedWidget.h>
#include <vtkSmartPointer.h>
#include <vtkVertexGlyphFilter.h>

// Custom
#include "Helpers.h"
#include "Types.h"

// Constructor
Form::Form()
{
  this->setupUi(this);

  this->LeftRenderer = vtkSmartPointer<vtkRenderer>::New();
  this->RightRenderer = vtkSmartPointer<vtkRenderer>::New();
  
  this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(this->LeftRenderer);
  this->qvtkWidgetRight->GetRenderWindow()->AddRenderer(this->RightRenderer);

  this->MovingImageActor = vtkSmartPointer<vtkImageActor>::New();
  this->MovingImageData = vtkSmartPointer<vtkImageData>::New();
  
  this->FixedImageActor = vtkSmartPointer<vtkImageActor>::New();
  this->FixedImageData = vtkSmartPointer<vtkImageData>::New();
  
  // Setup toolbar
  QIcon openIcon = QIcon::fromTheme("document-open");
  actionOpenFixedImage->setIcon(openIcon);
  this->toolBar->addAction(actionOpenFixedImage);
  
  actionOpenMovingImage->setIcon(openIcon);
  this->toolBar->addAction(actionOpenMovingImage);
  
  QIcon saveIcon = QIcon::fromTheme("document-save");
  actionSave->setIcon(saveIcon);
  this->toolBar->addAction(actionSave);

  this->FixedPointSelectionStyle2D = NULL;
  this->MovingPointSelectionStyle2D = NULL;
};

void Form::on_actionOpenMovingImage_activated()
{
   // Get a filename to open
  QString fileName = QFileDialog::getOpenFileName(this, "Open File", ".", "Image Files (*.png *.mhd *.tif)");

  std::cout << "Got filename: " << fileName.toStdString() << std::endl;
  if(fileName.toStdString().empty())
    {
    std::cout << "Filename was empty." << std::endl;
    return;
    }

  typedef itk::ImageFileReader<FloatVectorImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(fileName.toStdString());
  reader->Update();
  
  this->MovingImage = reader->GetOutput();

  if(this->chkRGB->isChecked())
    {
    Helpers::ITKImagetoVTKRGBImage(this->MovingImage, this->MovingImageData);
    }
  else
    {
    Helpers::ITKImagetoVTKMagnitudeImage(this->MovingImage, this->MovingImageData);
    }
  
  this->MovingImageActor->SetInput(this->MovingImageData);

  // Add Actor to renderer
  this->RightRenderer->AddActor(this->MovingImageActor);
  this->RightRenderer->ResetCamera();

  vtkSmartPointer<vtkInteractorStyleImage> interactorStyle =
      vtkSmartPointer<vtkInteractorStyleImage>::New();
  this->qvtkWidgetRight->GetRenderWindow()->GetInteractor()->SetInteractorStyle(interactorStyle);

  this->RightRenderer->ResetCamera();

  vtkSmartPointer<vtkPointPicker> pointPicker = vtkSmartPointer<vtkPointPicker>::New();
  this->qvtkWidgetRight->GetRenderWindow()->GetInteractor()->SetPicker(pointPicker);
  this->MovingPointSelectionStyle2D = vtkSmartPointer<PointSelectionStyle2D>::New();
  this->MovingPointSelectionStyle2D->SetCurrentRenderer(this->RightRenderer);
  this->qvtkWidgetRight->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->MovingPointSelectionStyle2D);

}

void Form::on_actionOpenFixedImage_activated()
{
  // Get a filename to open
  QString fileName = QFileDialog::getOpenFileName(this, "Open File", ".", "Image Files (*.png *.mhd *.tif)");

  std::cout << "Got filename: " << fileName.toStdString() << std::endl;
  if(fileName.toStdString().empty())
    {
    std::cout << "Filename was empty." << std::endl;
    return;
    }

  typedef itk::ImageFileReader<FloatVectorImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(fileName.toStdString());
  reader->Update();
  
  this->FixedImage = reader->GetOutput();

  if(this->chkRGB->isChecked())
    {
    Helpers::ITKImagetoVTKRGBImage(this->FixedImage, this->FixedImageData);
    }
  else
    {
    Helpers::ITKImagetoVTKMagnitudeImage(this->FixedImage, this->FixedImageData);
    }
  
  this->FixedImageActor->SetInput(this->FixedImageData);

  // Add Actor to renderer
  this->LeftRenderer->AddActor(this->FixedImageActor);
  this->LeftRenderer->ResetCamera();

  vtkSmartPointer<vtkInteractorStyleImage> interactorStyle =
      vtkSmartPointer<vtkInteractorStyleImage>::New();
  this->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetInteractorStyle(interactorStyle);


  vtkSmartPointer<vtkPointPicker> pointPicker = vtkSmartPointer<vtkPointPicker>::New();
  this->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetPicker(pointPicker);
  this->FixedPointSelectionStyle2D = vtkSmartPointer<PointSelectionStyle2D>::New();
  this->FixedPointSelectionStyle2D->SetCurrentRenderer(this->LeftRenderer);
  this->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->FixedPointSelectionStyle2D);
  
  this->LeftRenderer->ResetCamera();


}

void Form::on_actionSave_activated()
{
 
}
