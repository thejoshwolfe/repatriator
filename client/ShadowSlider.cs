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
                if (!(0 <= value && value <= maxPosition))
                    return;
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
                if (!(0 <= value && value <= maxPosition))
                    return;
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
                    return;
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

        public ShadowSlider()
        {
            ShadowPosition = 20;
            Position = 80;
            MaxPosition = 100;
        }

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

            if (orientation == System.Windows.Forms.Orientation.Vertical)
            {
                int pixelDelta = e.Y - mouseOrign.Y;
                int positionDelta = (int)(pixelDelta / pixelsPerPositionY);
                Position = positionOrigin + positionDelta;
            }
            else
            {
                // TODO
            }
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

        private const int endMargin = 3;
        private int trackHeight { get { return this.Height - endMargin * 2; } }
        private float pixelsPerPositionY { get { return trackHeight / (float)maxPosition; } }
        private int valueToY(int value)
        {
            return (int)(value * pixelsPerPositionY + endMargin);
        }
        protected override void OnPaint(PaintEventArgs e)
        {
            const int trackWidth = 4;
            const int thumbBreadth = 15;
            const int thumbSpan = 8;

            base.OnPaint(e);
            var g = e.Graphics;


            if (Orientation == System.Windows.Forms.Orientation.Vertical)
            {
                int centerX = this.Width / 2;
                // track
                g.FillRectangle(Brushes.LightGray, centerX - trackWidth / 2, endMargin, trackWidth, trackHeight);

                // shadow
                int shadowCenterY = valueToY(ShadowPosition);
                g.FillRectangle(Brushes.DarkGray, centerX - thumbBreadth / 2, shadowCenterY - thumbSpan / 2, thumbBreadth, thumbSpan);

                // thumb
                int thumbCenterY =  valueToY(Position);
                g.FillRectangle(Brushes.Black, centerX - thumbBreadth / 2, thumbCenterY - thumbSpan / 2, thumbBreadth, thumbSpan);
            }
            else
            {
                // TODO
            }
        }
    }
}
