#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkImageDataGeometryFilter.h>
#include <vtkWarpScalar.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkNamedColors.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkSliderWidget.h>
#include <vtkCallbackCommand.h>
#include <vtkProperty.h>
#include <vtkTransform.h>
#include <vtkTransformFilter.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkSphereSource.h>
#include <vtkTextureMapToSphere.h>
#include <vtkTexture.h>
#include <vtkJPEGReader.h>

/* // Callback for the slider to adjust the scale factor
class SliderCallback : public vtkCallbackCommand {
public:
    static SliderCallback* New() {
        return new SliderCallback;
    }

    void Execute(vtkObject* caller, unsigned long, void*) override {
        vtkSliderWidget* sliderWidget = reinterpret_cast<vtkSliderWidget*>(caller);
        double value = static_cast<vtkSliderRepresentation*>(sliderWidget->GetRepresentation())->GetValue();
        warpScalar->SetScaleFactor(value);
    }

    vtkSmartPointer<vtkWarpScalar> warpScalar;
};
 */
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <bathymetry.vtk>" << std::endl;
        return EXIT_FAILURE;
    }

    // Step 1: Read the image data from the .vtk file
    std::string filename = argv[1];
    vtkSmartPointer<vtkImageReader2Factory> readerFactory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkSmartPointer<vtkImageReader2> reader = readerFactory->CreateImageReader2(filename.c_str());
    reader->SetFileName(filename.c_str());
    reader->Update();

    vtkSmartPointer<vtkImageData> imageData = reader->GetOutput();

    // Step 2: Convert image data to polydata
    vtkSmartPointer<vtkImageDataGeometryFilter> geometryFilter = vtkSmartPointer<vtkImageDataGeometryFilter>::New();
    geometryFilter->SetInputData(imageData);
    geometryFilter->Update();

    // Step 3: Apply vtkWarpScalar to create the height map
    vtkSmartPointer<vtkWarpScalar> warpScalar = vtkSmartPointer<vtkWarpScalar>::New();
    warpScalar->SetInputConnection(geometryFilter->GetOutputPort());
    warpScalar->SetScaleFactor(0.02);  // Adjust initial scale factor for spherical earth

    // Step 4: Create a spherical transformation
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Identity();

    // Convert Cartesian to Spherical Coordinates
    vtkSmartPointer<vtkTransformFilter> transformFilter = vtkSmartPointer<vtkTransformFilter>::New();
    transformFilter->SetInputConnection(warpScalar->GetOutputPort());
    transformFilter->SetTransform(transform);

    // Step 5: Apply the transform to wrap the height map onto a sphere
    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(6371); // Approximate radius of the Earth in kilometers
    sphereSource->SetThetaResolution(360);
    sphereSource->SetPhiResolution(180);

    vtkSmartPointer<vtkTextureMapToSphere> textureMap = vtkSmartPointer<vtkTextureMapToSphere>::New();
    textureMap->SetInputConnection(transformFilter->GetOutputPort());
    textureMap->PreventSeamOn();

    // Step 6: Create a color map and texture
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(textureMap->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // Step 7: Create a renderer and render window
    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);

    renderer->AddActor(actor);
    renderer->SetBackground(0.1, 0.2, 0.4);  // Dark blue background
    renderWindow->Render();

    /* // Step 8: Create a slider widget for adjusting the scale factor
    vtkSmartPointer<vtkSliderRepresentation2D> sliderRep = vtkSmartPointer<vtkSliderRepresentation2D>::New();
    sliderRep->SetMinimumValue(0.0);
    sliderRep->SetMaximumValue(50.0);
    sliderRep->SetValue(1.0);  // Initial scale factor
    sliderRep->SetTitleText("Scale Factor");

    sliderRep->GetSliderProperty()->SetColor(1, 0, 0);  // Slider color
    sliderRep->GetTitleProperty()->SetColor(1, 1, 1);   // Title color
    sliderRep->GetLabelProperty()->SetColor(1, 1, 1);   // Label color
    sliderRep->GetTubeProperty()->SetColor(1, 1, 0);    // Tube color
    sliderRep->GetCapProperty()->SetColor(1, 1, 0);     // Cap color

    sliderRep->SetSliderLength(0.02);
    sliderRep->SetSliderWidth(0.03);
    sliderRep->SetEndCapLength(0.01);
    sliderRep->SetEndCapWidth(0.02);
    sliderRep->SetTubeWidth(0.005);

    vtkSmartPointer<vtkSliderWidget> sliderWidget = vtkSmartPointer<vtkSliderWidget>::New();
    sliderWidget->SetInteractor(renderWindowInteractor);
    sliderWidget->SetRepresentation(sliderRep);

    vtkSmartPointer<SliderCallback> sliderCallback = vtkSmartPointer<SliderCallback>::New();
    sliderCallback->warpScalar = warpScalar;
    sliderWidget->AddObserver(vtkCommand::InteractionEvent, sliderCallback);

    sliderWidget->EnabledOn(); */

    // Step 9: Start the interaction
    renderWindowInteractor->Start();

    return EXIT_SUCCESS;
}
