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

            refreshTransforms();
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
            angleY += delta.X * 1;
            angleX += delta.Y * 1;
            lastMouseLocation = currentMoustLocation;

            refreshTransforms();
        }

        private void refreshTransforms()
        {
            innerRotation.Angle = angleX;
            outterRotation.Angle = angleY;
        }
    }
}
