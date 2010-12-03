using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Windows.Forms;

namespace repatriator_client
{
    class ShadowMinimap : Control
    {
        public event Action positionChosen;

        private Point shadowPosition = new Point(20, 20);
        public Point ShadowPosition
        {
            get { return shadowPosition; }
            set
            {
                if (value.X < 0)
                    value.X = 0;
                else if (value.X > maxPosition.X)
                    value.X = maxPosition.X;
                if (value.Y < 0)
                    value.Y = 0;
                else if (value.Y > maxPosition.Y)
                    value.Y = maxPosition.Y;
                shadowPosition = value;
                this.Refresh();
            }
        }
        private Point position = new Point(80, 80);
        public Point Position
        {
            get { return position; }
            set
            {
                if (value.X < 0)
                    value.X = 0;
                else if (value.X > maxPosition.X)
                    value.X = maxPosition.X;
                if (value.Y < 0)
                    value.Y = 0;
                else if (value.Y > maxPosition.Y)
                    value.Y = maxPosition.Y;
                position = value;
                this.Refresh();
            }
        }
        private Point maxPosition = new Point(100, 100);
        public Point MaxPosition
        {
            get { return maxPosition; }
            set
            {
                if (value.X <= 0)
                    value.X = 1;
                if (value.Y <= 0)
                    value.Y = 1;
                maxPosition = value;
                this.Refresh();
            }
        }

        // enable double buffering
        protected override bool DoubleBuffered { get { return true; } set { base.DoubleBuffered = value; } }

        private Point mouseOrign;
        private Point positionOrigin;
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

            Point pixelDelta = new Point(e.X - mouseOrign.X, e.Y - mouseOrign.Y);
            Point positionDelta = new Point((int)(pixelDelta.X / pixelsPerPositionX), (int)(pixelDelta.Y / pixelsPerPositionY));
            Position = Utils.add(positionOrigin, positionDelta);
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

        private readonly Point thumbSize = new Point(20, 16);
        private readonly Point margin = new Point(10, 8);
        private int fieldWidth { get { return this.Width - margin.X * 2; } }
        private int fieldHeight { get { return this.Height - margin.Y * 2; } }
        private float pixelsPerPositionX { get { return fieldWidth / (float)maxPosition.X; } }
        private float pixelsPerPositionY { get { return fieldHeight / (float)maxPosition.Y; } }
        private Point valueToPoint(Point value) { return new Point((int)(value.X * pixelsPerPositionX + margin.X), (int)(value.Y * pixelsPerPositionY + margin.Y)); }
        protected override void OnPaint(PaintEventArgs e)
        {
            const int borderWidth = 4;

            base.OnPaint(e);
            Graphics g = e.Graphics;

            // border
            g.DrawRectangle(new Pen(Color.LightGray, borderWidth), borderWidth / 2, borderWidth / 2, this.Width - borderWidth, this.Height - borderWidth);

            // shadow
            Point shadowCenter = valueToPoint(shadowPosition);
            g.FillRectangle(Brushes.DarkGray, shadowCenter.X - thumbSize.X / 2, shadowCenter.Y - thumbSize.Y / 2, thumbSize.X, thumbSize.Y);

            // thumb
            Point positionCenter = valueToPoint(position);
            g.FillRectangle(Brushes.Black, positionCenter.X - thumbSize.X / 2, positionCenter.Y - thumbSize.Y / 2, thumbSize.X, thumbSize.Y);
        }
    }
}
