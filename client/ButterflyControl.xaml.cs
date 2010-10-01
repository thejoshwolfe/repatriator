using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Media3D;
using System.Windows.Media;
using System.Linq;
using System;
using System.Windows.Input;

namespace repatriator_client
{
    public partial class ButterflyControl : UserControl
    {
        private const double angleStep = 1.0;
        private double angleX = 0.0;
        private double angleY = 0.0;
        public ButterflyControl()
        {
            InitializeComponent();

            refreshTransforms();
        }

        private bool dragging = false;
        private System.Drawing.Point lockCursorPosition;
        private System.Drawing.Point restoreCursorPosition;

        private void mouseIntercepterCanvas_MouseDown(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            dragging = true;

            // store the center of the control
            System.Windows.Point relativeCenter = new System.Windows.Point(mouseIntercepterCanvas.ActualWidth / 2, mouseIntercepterCanvas.ActualHeight / 2);
            System.Windows.Point absoluteCenter = PointToScreen(relativeCenter);
            lockCursorPosition = new System.Drawing.Point((int)absoluteCenter.X, (int)absoluteCenter.Y);

            // hide the cursor
            System.Windows.Forms.Cursor.Hide();

            // remember the user's cursor position so we can restore it later
            restoreCursorPosition = System.Windows.Forms.Cursor.Position;
            // start with the cursor in the center
            System.Windows.Forms.Cursor.Position = lockCursorPosition;
        }
        private void mouseIntercepterCanvas_MouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            dragging = false;

            // restore position of cursor to where the user wanted it
            System.Windows.Forms.Cursor.Position = restoreCursorPosition;

            // show the cursor
            System.Windows.Forms.Cursor.Show();
        }
        private void mouseIntercepterCanvas_MouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            if (!dragging)
                return;

            // calculate the movement (in absolute coordinates)
            System.Drawing.Point currentMouseLocation = System.Windows.Forms.Cursor.Position;
            int deltaX = currentMouseLocation.X - lockCursorPosition.X;
            int deltaY = currentMouseLocation.Y - lockCursorPosition.Y;
            angleY += deltaX * angleStep;
            angleX += deltaY * angleStep;
            // hold the cursor in the center of the control
            System.Windows.Forms.Cursor.Position = lockCursorPosition;

            refreshTransforms();
        }

        private void refreshTransforms()
        {
            innerRotation.Angle = angleX;
            outterRotation.Angle = angleY;
        }
    }
}
