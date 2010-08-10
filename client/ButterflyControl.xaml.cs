using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Media3D;
using System.Windows.Media;
using System.Linq;
using System;

namespace repatriator_client
{
    public partial class ButterflyControl : UserControl
    {
        private double angleX = 0.0;
        private double angleY = 0.0;
        public ButterflyControl()
        {
            InitializeComponent();

            // http://www.kindohm.com/technical/WPF3DTutorial.htm
            // http://www.switchonthecode.com/tutorials/wpf-tutorial-using-wpf-in-winforms

            createEverything();
        }
        private void createEverything()
        {
            Model3DGroup cube = new Model3DGroup();
            Point3D p0 = new Point3D(-5, -5, -5);
            Point3D p1 = new Point3D( 5, -5, -5);
            Point3D p2 = new Point3D( 5, -5,  5);
            Point3D p3 = new Point3D(-5, -5,  5);
            Point3D p4 = new Point3D(-5,  5, -5);
            Point3D p5 = new Point3D( 5,  5, -5);
            Point3D p6 = new Point3D( 5,  5,  5);
            Point3D p7 = new Point3D(-5,  5,  5);
            //front side triangles
            cube.Children.Add(CreateTriangleModel(p3, p2, p6));
            cube.Children.Add(CreateTriangleModel(p3, p6, p7));
            //right side triangles
            cube.Children.Add(CreateTriangleModel(p2, p1, p5));
            cube.Children.Add(CreateTriangleModel(p2, p5, p6));
            //back side triangles
            cube.Children.Add(CreateTriangleModel(p1, p0, p4));
            cube.Children.Add(CreateTriangleModel(p1, p4, p5));
            //left side triangles
            cube.Children.Add(CreateTriangleModel(p0, p3, p7));
            cube.Children.Add(CreateTriangleModel(p0, p7, p4));
            //top side triangles
            cube.Children.Add(CreateTriangleModel(p7, p6, p5));
            cube.Children.Add(CreateTriangleModel(p7, p5, p4));
            //bottom side triangles
            cube.Children.Add(CreateTriangleModel(p2, p3, p0));
            cube.Children.Add(CreateTriangleModel(p2, p0, p1));

            RotateTransform3D rotateX = new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(1, 0, 0), angleX * 180));
            cube.Transform = rotateX;
            Model3DGroup cube2 = new Model3DGroup();
            cube2.Children.Add(cube);
            RotateTransform3D rotateY = new RotateTransform3D(new AxisAngleRotation3D(new Vector3D(0, 1, 0), angleY * 180));
            cube2.Transform = rotateY;
            ModelVisual3D model = new ModelVisual3D();
            model.Content = cube2;
            foreach (Visual3D child in mainViewport.Children.ToArray())
            {
                if (child is ModelVisual3D && ((ModelVisual3D)child).Content is Model3DGroup)
                {
                    mainViewport.Children.Remove(child);
                }
            }

            mainViewport.Children.Add(model);
        }
        private Model3DGroup CreateTriangleModel(Point3D p0, Point3D p1, Point3D p2)
        {
            MeshGeometry3D mesh = new MeshGeometry3D();
            mesh.Positions.Add(p0);
            mesh.Positions.Add(p1);
            mesh.Positions.Add(p2);
            mesh.TriangleIndices.Add(0);
            mesh.TriangleIndices.Add(1);
            mesh.TriangleIndices.Add(2);
            Vector3D normal = CalculateNormal(p0, p1, p2);
            mesh.Normals.Add(normal);
            mesh.Normals.Add(normal);
            mesh.Normals.Add(normal);
            Material material = new DiffuseMaterial(new SolidColorBrush(Colors.DarkKhaki));
            GeometryModel3D model = new GeometryModel3D(mesh, material);
            Model3DGroup group = new Model3DGroup();
            group.Children.Add(model);
            return group;
        }
        private Vector3D CalculateNormal(Point3D p0, Point3D p1, Point3D p2)
        {
            Vector3D v0 = new Vector3D(p1.X - p0.X, p1.Y - p0.Y, p1.Z - p0.Z);
            Vector3D v1 = new Vector3D(p2.X - p1.X, p2.Y - p1.Y, p2.Z - p1.Z);
            return Vector3D.CrossProduct(v0, v1);
        }

        private bool dragging = false;
        private Point lastMouseLocation;
        private void mouseIntercepterCanvas_MouseDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            dragging = true;
            lastMouseLocation = e.GetPosition(this);
        }
        private void mouseIntercepterCanvas_MouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            dragging = false;
        }
        private void mouseIntercepterCanvas_MouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (!dragging)
                return;
            Point currentMoustLocation = e.GetPosition(this);
            Vector delta = currentMoustLocation - lastMouseLocation;
            angleY += delta.X * 0.003;
            angleX += delta.Y * 0.003;
            lastMouseLocation = currentMoustLocation;

            createEverything();
        }
    }
}
