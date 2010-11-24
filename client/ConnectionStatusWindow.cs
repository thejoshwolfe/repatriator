using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace repatriator_client
{
    public partial class ConnectionStatusWindow : Form
    {
        private readonly ConnectionManager connectionManager;
        public bool result = false;
        public ConnectionStatusWindow(ConnectionManager connectionManager)
        {
            this.connectionManager = connectionManager;
            InitializeComponent();

            connectionManager.connectionUpdate += new Action<ConnectionStatus>(connectionManager_connectionUpdate);
            connectionManager.loginFinished += new Action<LoginStatus>(connectionManager_loginFinished);

            connectionManager.start();
        }

        private void connectionManager_loginFinished(LoginStatus status)
        {
            switch (status)
            {
                case LoginStatus.Success:
                    result = true;
                    BeginInvoke(new Action(delegate()
                    {
                        Close();
                    }));
                    break;
            }
        }

        private void connectionManager_connectionUpdate(ConnectionStatus status)
        {
            switch (status)
            {
                case ConnectionStatus.Success:
                    break;
                case ConnectionStatus.Trouble:
                    break;
            }
        }
    }
}
