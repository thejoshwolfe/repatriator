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

        protected override void OnPaint(PaintEventArgs e)
        {
            const int endMargin = 3;
            const int trackWidth = 4;
            const int thumbBreadth = 15;
            const int thumbSpan = 8;

            base.OnPaint(e);
            var g = e.Graphics;


            if (Orientation == System.Windows.Forms.Orientation.Vertical)
            {
                int centerX = this.Width / 2;
                int trackHeight = this.Height - endMargin * 2;
                // track
                g.FillRectangle(Brushes.LightGray, centerX - trackWidth / 2, endMargin, trackWidth, trackHeight);

                float scale = trackHeight / (float)MaxPosition;
                // shadow
                int shadowCenterY = (int)(ShadowPosition * scale + endMargin);
                g.FillRectangle(Brushes.DarkGray, centerX - thumbBreadth / 2, shadowCenterY - thumbSpan / 2, thumbBreadth, thumbSpan);

                // thumb
                int thumbCenterY = (int)(Position * scale + endMargin);
                g.FillRectangle(Brushes.Black, centerX - thumbBreadth / 2, thumbCenterY - thumbSpan / 2, thumbBreadth, thumbSpan);
            }
            else
            {
                // TODO
            }
        }
    }
}
