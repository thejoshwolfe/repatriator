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
        private readonly ConnectionManager connectionManager;
        public AdminWindow(ConnectionManager connectionManager)
        {
            this.connectionManager = connectionManager;

            InitializeComponent();
        }

        private void cancelButton_Click(object sender, EventArgs e)
        {
            Close();
        }
    }
}

