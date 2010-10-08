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
    public partial class AdminWindow : Form
    {
        private static AdminWindow instance = null;
        public static void showInstance()
        {
            // do we need thread safety?
            if (instance == null)
            {
                instance = new AdminWindow();
                instance.Show();
            }
            else
                instance.Focus();
        }

        private ConnectionManager connectionManager;
        public AdminWindow()
        {
            InitializeComponent();
        }

        private void AdminWindow_FormClosed(object sender, FormClosedEventArgs e)
        {
            instance = null;
        }

        private void connectButton_Click(object sender, EventArgs e)
        {
            string serverString = serverText.Text;
            string[] nameAndPort = serverString.Split(":");
            if (nameAndPort.Length != 2)
                return;
            string serverName = nameAndPort[0];
            int serverPort;
            if (!int.TryParse(nameAndPort[1], out serverPort))
                return;
            string userName = userNameText.Text;
            string password = passwordText.Text;
            connectionManager.setEverything(serverName, serverPort, userName, password, null);
            // maybeStartConnectionManager();
        }
    }
}
