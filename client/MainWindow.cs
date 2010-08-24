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
        private ConnectionManager connectionManager;

        public MainWindow()
        {
            InitializeComponent();

            connectionManager = ConnectionManager.load();
            // auto connect if we're good to go
            if (connectionManager.hasValidSettings())
                connectionManager.start();
        }
    }
}
