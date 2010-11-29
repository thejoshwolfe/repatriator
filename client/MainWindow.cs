using System;
using System.Drawing;
using System.Net.Sockets;
using System.Windows.Forms;
using System.Threading;
using System.IO;

namespace repatriator_client
{
    public partial class MainWindow : Form
    {
        private ButterflyControl butterflyControl;
        private ConnectionManager connectionManager;

        public MainWindow(ConnectionManager connectionManager)
        {
            this.connectionManager = connectionManager;

            InitializeComponent();

            butterflyControl = new ButterflyControl();
            butterflyControl.AnglesMoved += new Action(butterflyControl_AnglesMoved);
            butterflyElemtnHost.Child = butterflyControl;
        }
        private void butterflyControl_AnglesMoved()
        {
            butterflySliderX.Value = butterflyControl.AngleX;
            // invert the Y. maybe we shouldn't do this.
            butterflySliderY.Value = butterflySliderY.Maximum - butterflyControl.AngleY;
        }
    }
}
