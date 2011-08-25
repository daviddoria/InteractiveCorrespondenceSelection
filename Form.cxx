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
//#include "itkImageFileWriter.h"
//#include "itkRegionOfInterestImageFilter.h"
#include "itkVector.h"

// Qt
#include <QFileDialog>
#include <QIcon>
#include <QTextEdit>

// VTK
#include <vtkActor.h>
#include <vtkActor2D.h>
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


void Form::on_actionHelp_activated()
{
  QTextEdit* help=new QTextEdit();

  help->setReadOnly(true);
  help->append("<h1>Interaction</h1>\
  Hold the right mouse button and drag to zoom in and out. <br/>\
  Hold the middle mouse button and drag to pan the image. <p/>\
  <h1>Selecting correspondences</h1>\
  Click the left mouse button to select a keypoint.<br/> <p/>\
  <h1>Saving keypoints</h1>\
  The same number of keypoints must be selected in both the images before the points can be saved."
  );
  help->show();
}

// Constructor
Form::Form()
{
  this->setupUi(this);

  this->LeftRenderer = vtkSmartPointer<vtkRenderer>::New();
  this->RightRenderer = vtkSmartPointer<vtkRenderer>::New();
  
  this->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(this->LeftRenderer);
  this->qvtkWidgetRight->GetRenderWindow()->AddRenderer(this->RightRenderer);

  this->Image1Actor = vtkSmartPointer<vtkImageActor>::New();
  this->Image1Data = vtkSmartPointer<vtkImageData>::New();
  
  this->Image2Actor = vtkSmartPointer<vtkImageActor>::New();
  this->Image2Data = vtkSmartPointer<vtkImageData>::New();

  // Setup icons
  QIcon saveIcon = QIcon::fromTheme("document-save");
  QIcon openIcon = QIcon::fromTheme("document-open");

  // Setup left toolbar
  //toolBar_Left->setAllowedAreas(Qt::LeftToolBarArea);
  //toolBar_Left->setOrientation(Qt::Vertical);
  //this->addToolBar(Qt::LeftToolBarArea, toolBar_Left );
  
  actionOpenImage1->setIcon(openIcon);
  this->toolBar_Left->addAction(actionOpenImage1);
  
  actionSaveImage1Points->setIcon(saveIcon);
  this->toolBar_Left->addAction(actionSaveImage1Points);
  
  // Setup right toolbar
  //toolBar_Right->setAllowedAreas(Qt::RightToolBarArea);
  
  actionOpenImage2->setIcon(openIcon);
  this->toolBar_Right->addAction(actionOpenImage2);
  
  actionSaveImage2Points->setIcon(saveIcon);
  this->toolBar_Right->addAction(actionSaveImage2Points);

  // Initialize
  this->Image1SelectionStyle2D = NULL;
  this->Image2SelectionStyle2D = NULL;
};

void Form::on_btnDeleteLastImage1_clicked()
{
  this->LeftRenderer->RemoveViewProp( this->Image1SelectionStyle2D->Numbers[this->Image1SelectionStyle2D->Numbers.size() - 1]);
  this->LeftRenderer->RemoveViewProp( this->Image1SelectionStyle2D->Points[this->Image1SelectionStyle2D->Points.size() - 1]);
  this->Image1SelectionStyle2D->Numbers.erase(this->Image1SelectionStyle2D->Numbers.end()-1);
  this->Image1SelectionStyle2D->Points.erase(this->Image1SelectionStyle2D->Points.end()-1);
  this->Image1SelectionStyle2D->Coordinates.erase(this->Image1SelectionStyle2D->Coordinates.end()-1);
  this->qvtkWidgetLeft->GetRenderWindow()->Render();
}

void Form::on_btnDeleteLastImage2_clicked()
{
  this->RightRenderer->RemoveViewProp( this->Image2SelectionStyle2D->Numbers[this->Image2SelectionStyle2D->Numbers.size() - 1]);
  this->RightRenderer->RemoveViewProp( this->Image2SelectionStyle2D->Points[this->Image2SelectionStyle2D->Points.size() - 1]);
  this->Image2SelectionStyle2D->Numbers.erase(this->Image2SelectionStyle2D->Numbers.end()-1);
  this->Image2SelectionStyle2D->Points.erase(this->Image2SelectionStyle2D->Points.end()-1);
  this->Image2SelectionStyle2D->Coordinates.erase(this->Image2SelectionStyle2D->Coordinates.end()-1);
  this->qvtkWidgetRight->GetRenderWindow()->Render();
}

void Form::on_btnDeleteAllImage1_clicked()
{
  this->Image1SelectionStyle2D->RemoveAllPoints();
  this->qvtkWidgetLeft->GetRenderWindow()->Render();
}

void Form::on_btnDeleteAllImage2_clicked()
{
  this->Image2SelectionStyle2D->RemoveAllPoints();
  this->qvtkWidgetRight->GetRenderWindow()->Render();
}

void Form::on_actionOpenImage1_activated()
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
  
  this->Image1 = reader->GetOutput();

  if(this->chkRGB->isChecked())
    {
    Helpers::ITKImagetoVTKRGBImage(this->Image1, this->Image1Data);
    }
  else
    {
    Helpers::ITKImagetoVTKMagnitudeImage(this->Image1, this->Image1Data);
    }
  
  this->Image1Actor->SetInput(this->Image1Data);

  // Add Actor to renderer
  this->LeftRenderer->AddActor(this->Image1Actor);
  this->LeftRenderer->ResetCamera();

  vtkSmartPointer<vtkInteractorStyleImage> interactorStyle =
      vtkSmartPointer<vtkInteractorStyleImage>::New();
  this->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetInteractorStyle(interactorStyle);

  this->LeftRenderer->ResetCamera();

  vtkSmartPointer<vtkPointPicker> pointPicker = vtkSmartPointer<vtkPointPicker>::New();
  this->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetPicker(pointPicker);
  this->Image1SelectionStyle2D = vtkSmartPointer<PointSelectionStyle2D>::New();
  this->Image1SelectionStyle2D->SetCurrentRenderer(this->LeftRenderer);
  this->qvtkWidgetLeft->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->Image1SelectionStyle2D);

}

void Form::on_actionOpenImage2_activated()
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
  
  this->Image2 = reader->GetOutput();

  if(this->chkRGB->isChecked())
    {
    Helpers::ITKImagetoVTKRGBImage(this->Image2, this->Image2Data);
    }
  else
    {
    Helpers::ITKImagetoVTKMagnitudeImage(this->Image2, this->Image2Data);
    }
  
  this->Image2Actor->SetInput(this->Image2Data);

  // Add Actor to renderer
  this->RightRenderer->AddActor(this->Image2Actor);
  this->RightRenderer->ResetCamera();

  vtkSmartPointer<vtkInteractorStyleImage> interactorStyle =
      vtkSmartPointer<vtkInteractorStyleImage>::New();
  this->qvtkWidgetRight->GetRenderWindow()->GetInteractor()->SetInteractorStyle(interactorStyle);


  vtkSmartPointer<vtkPointPicker> pointPicker = vtkSmartPointer<vtkPointPicker>::New();
  this->qvtkWidgetRight->GetRenderWindow()->GetInteractor()->SetPicker(pointPicker);
  this->Image2SelectionStyle2D = vtkSmartPointer<PointSelectionStyle2D>::New();
  this->Image2SelectionStyle2D->SetCurrentRenderer(this->RightRenderer);
  this->qvtkWidgetRight->GetRenderWindow()->GetInteractor()->SetInteractorStyle(this->Image2SelectionStyle2D);
  
  this->RightRenderer->ResetCamera();


}

void Form::on_actionSaveImage1Points_activated()
{
  if(!this->Image1SelectionStyle2D || !this->Image2SelectionStyle2D)
    {
    std::cerr << "You must have loaded and selected points from both images!" << std::endl;
    return;
    }

  if(this->Image1SelectionStyle2D->Numbers.size() !=
     this->Image2SelectionStyle2D->Numbers.size())
  {
    std::cerr << "The number of image1 correspondences must match the number of image2 correspondences!" << std::endl;
    return;
  }

  QString fileName = QFileDialog::getSaveFileName(this, "Save File", ".", "Text Files (*.txt)");
  std::cout << "Got filename: " << fileName.toStdString() << std::endl;
  if(fileName.toStdString().empty())
    {
    std::cout << "Filename was empty." << std::endl;
    return;
    }

  std::ofstream fout(fileName.toStdString().c_str());

  for(unsigned int i = 0; i < this->Image1SelectionStyle2D->Coordinates.size(); i++)
    {
    fout << this->Image1SelectionStyle2D->Coordinates[i].x << " " << this->Image1SelectionStyle2D->Coordinates[i].y << std::endl;
    }
  fout.close();
}

void Form::on_actionSaveImage2Points_activated()
{
  if(!this->Image1SelectionStyle2D || !this->Image2SelectionStyle2D)
    {
    std::cerr << "You must have loaded and selected points from both images!" << std::endl;
    return;
    }

  if(this->Image1SelectionStyle2D->Numbers.size() !=
     this->Image2SelectionStyle2D->Numbers.size())
    {
    std::cerr << "The number of image1 correspondences must match the number of image2 correspondences!" << std::endl;
    return;
    }

  QString fileName = QFileDialog::getSaveFileName(this, "Save File", ".", "Text Files (*.txt)");
  std::cout << "Got filename: " << fileName.toStdString() << std::endl;
  if(fileName.toStdString().empty())
    {
    std::cout << "Filename was empty." << std::endl;
    return;
    }

  std::ofstream fout(fileName.toStdString().c_str());

  for(unsigned int i = 0; i < this->Image2SelectionStyle2D->Coordinates.size(); i++)
    {
    fout << this->Image2SelectionStyle2D->Coordinates[i].x << " " << this->Image2SelectionStyle2D->Coordinates[i].y << std::endl;
    }
  fout.close();
}
