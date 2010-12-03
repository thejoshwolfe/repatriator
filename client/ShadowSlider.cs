using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Drawing;

namespace repatriator_client
{
    public class ShadowSlider : Control
    {
        public event Action positionChosen;
        private int shadowPosition = 20;
        public int ShadowPosition
        {
            get { return shadowPosition; }
            set
            {
                if (value < 0)
                    value = 0;
                else if (value > maxPosition)
                    value = maxPosition;
                shadowPosition = value;
                this.Refresh();
            }
        }
        private int position = 80;
        public int Position
        {
            get { return position; }
            set
            {
                if (value < 0)
                    value = 0;
                else if (value > maxPosition)
                    value = maxPosition;
                position = value;
                this.Refresh();
            }
        }
        private int maxPosition = 100;
        public int MaxPosition
        {
            get { return maxPosition; }
            set
            {
                if (value <= 0)
                    value = 1;
                maxPosition = value;
                this.Refresh();
            }
        }
        private System.Windows.Forms.Orientation orientation;
        public Orientation Orientation
        {
            get { return orientation; }
            set
            {
                orientation = value;
                this.Refresh();
            }
        }

        // enable double buffering
        protected override bool DoubleBuffered { get { return true; } set { base.DoubleBuffered = value; } }

        private Point mouseOrign;
        private int positionOrigin;
        private bool mouseIsDown = false;
        protected override void OnMouseDown(MouseEventArgs e)
        {
            base.OnMouseDown(e);

            mouseOrign.X = e.X;
            mouseOrign.Y = e.Y;
            positionOrigin = position;
            mouseIsDown = true;
        }
        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);
            if (!mouseIsDown)
                return;

            int positionDelta;
            if (orientation == System.Windows.Forms.Orientation.Vertical)
            {
                int pixelDelta = e.Y - mouseOrign.Y;
                positionDelta = (int)(pixelDelta / pixelsPerPositionY);
            }
            else
            {
                int pixelDelta = e.X - mouseOrign.X;
                positionDelta = (int)(pixelDelta / pixelsPerPositionX);
            }
            Position = positionOrigin + positionDelta;
        }
        protected override void OnMouseUp(MouseEventArgs e)
        {
            base.OnMouseUp(e);
            if (!mouseIsDown)
                return;

            mouseIsDown = false;
            if (positionOrigin != position)
                if (positionChosen != null)
                    positionChosen();
        }

        private const int endMargin = 4;
        private int trackWidth { get { return this.Width - endMargin * 2; } }
        private int trackHeight { get { return this.Height - endMargin * 2; } }
        private float pixelsPerPositionX { get { return trackWidth / (float)maxPosition; } }
        private float pixelsPerPositionY { get { return trackHeight / (float)maxPosition; } }
        private int valueToX(int value) { return (int)(value * pixelsPerPositionX + endMargin); }
        private int valueToY(int value) { return (int)(value * pixelsPerPositionY + endMargin); }
        protected override void OnPaint(PaintEventArgs e)
        {
            const int trackBreadth = 4;
            const int thumbBreadth = 15;
            const int thumbSpan = endMargin * 2;

            base.OnPaint(e);
            Graphics g = e.Graphics;


            if (Orientation == System.Windows.Forms.Orientation.Vertical)
            {
                int centerX = this.Width / 2;
                // track
                g.FillRectangle(Brushes.LightGray, centerX - trackBreadth / 2, endMargin, trackBreadth, trackHeight);

                // shadow
                int shadowCenterY = valueToY(shadowPosition);
                g.FillRectangle(Brushes.DarkGray, centerX - thumbBreadth / 2, shadowCenterY - thumbSpan / 2, thumbBreadth, thumbSpan);

                // thumb
                int thumbCenterY = valueToY(position);
                g.FillRectangle(Brushes.Black, centerX - thumbBreadth / 2, thumbCenterY - thumbSpan / 2, thumbBreadth, thumbSpan);
            }
            else
            {
                int centerY = this.Height / 2;
                // track
                g.FillRectangle(Brushes.LightGray, endMargin, centerY - trackBreadth / 2, trackWidth, trackBreadth);

                // shadow
                int shadowCenterX = valueToX(shadowPosition);
                g.FillRectangle(Brushes.DarkGray, shadowCenterX - thumbSpan / 2, centerY - thumbBreadth / 2, thumbSpan, thumbBreadth);

                // thumb
                int thumbCenterX = valueToX(position);
                g.FillRectangle(Brushes.Black, thumbCenterX - thumbSpan / 2, centerY - thumbBreadth / 2, thumbSpan, thumbBreadth);
            }
        }
    }
}
