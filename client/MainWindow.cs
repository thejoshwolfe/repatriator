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
            butterflyElemtnHost.Child = butterflyControl;
        }
        private void MainWindow_Load(object sender, EventArgs e)
        {
            // attach handlers after the gui has initialized
            connectionManager.fullUpdated += new Action(connectionManager_fullUpdated);
            connectionManager.directoryListUpdated += new Action(connectionManager_directoryListUpdated);

            // delete dummy images
            directoryImageList.Images.Clear();

            connectionManager.refreshDirectoryList();
        }

        private void connectionManager_directoryListUpdated()
        {
            BeginInvoke(new Action(updateDirectory));
        }

        private void updateDirectory()
        {
            directoryListView.Items.Clear();
            directoryImageList.Images.Clear();
            for (int i = 0; i < connectionManager.directoryList.Length; i++)
            {
                DirectoryItem directoryItem = connectionManager.directoryList[i];
                Image thumbnail = directoryItem.thumbnail;
                if (thumbnail == null)
                    thumbnail = this.Icon.ToBitmap();
                directoryImageList.Images.Add(thumbnail);
                directoryListView.Items.Add(directoryItem.filename, i);
            }
        }
        private void connectionManager_fullUpdated()
        {
            BeginInvoke(new Action(delegate()
            {
                Image previousImage = liveViewPictureBox.Image;
                liveViewPictureBox.Image = connectionManager.image;
                if (previousImage != null)
                    previousImage.Dispose();
            }));
        }

        private void takePictureButton_Click(object sender, EventArgs e)
        {
            connectionManager.takePicture();
        }

        private void MainWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            connectionManager.close();
        }
    }
}

