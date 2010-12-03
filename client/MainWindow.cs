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
        private ConnectionSettings account;
        private int downloadTotalCount, downloadSoFar;

        public MainWindow(ConnectionManager connectionManager, ConnectionSettings account)
        {
            this.connectionManager = connectionManager;
            this.account = account;

            InitializeComponent();

            butterflyControl = new ButterflyControl();
            butterflyElemtnHost.Child = butterflyControl;
        }

        private void MainWindow_Load(object sender, EventArgs e)
        {
            // attach handlers after the gui has initialized
            connectionManager.fullUpdated += new Action(connectionManager_fullUpdated);
            connectionManager.directoryListUpdated += new Action(connectionManager_directoryListUpdated);
            connectionManager.downloadProgressed += new Action<float>(connectionManager_downloadProgressed);

            // delete dummy images
            directoryImageList.Images.Clear();

            connectionManager.refreshDirectoryList();

            enableCorrectControls();
        }

        private void connectionManager_downloadProgressed(float value)
        {
            BeginInvoke(new Action(delegate()
            {
                downloadAllButton.Text = ((downloadSoFar + value) / downloadTotalCount).ToString("0.00");
                if (value == 1.0)
                    downloadSoFar++;
            }));
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

                orbitSliderA.ShadowPosition = (int)connectionManager.motorPositions[0];
                orbitSliderB.ShadowPosition = (int)connectionManager.motorPositions[1];
                shadowMinimap.ShadowPosition = new Point((int)connectionManager.motorPositions[2], (int)connectionManager.motorPositions[3]);
                liftSliderZ.ShadowPosition = (int)connectionManager.motorPositions[4];
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

        private void downloadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (!checkDownloadDirectory())
                return;
            // send download file messages for each file
            downloadTotalCount = directoryListView.SelectedIndices.Count;
            downloadSoFar = 0;
            for (int i = 0; i < directoryListView.SelectedIndices.Count; ++i)
            {
                string filename = directoryListView.Items[directoryListView.SelectedIndices[i]].Text;
                connectionManager.downloadFile(filename);
            }
        }
        private void downloadAllButton_Click(object sender, EventArgs e)
        {
            if (!checkDownloadDirectory())
                return;
            downloadTotalCount = directoryListView.Items.Count;
            downloadSoFar = 0;
            for (int i = 0; i < directoryListView.Items.Count; ++i)
            {
                string filename = directoryListView.Items[i].Text;
                connectionManager.downloadFile(filename);
            }
        }
        private bool checkDownloadDirectory()
        {
            // make sure we have a download dir
            if (account.downloadDirectory != null)
                return true;
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            if (dialog.ShowDialog(this) == System.Windows.Forms.DialogResult.Cancel)
                return false;
            account.downloadDirectory = dialog.SelectedPath;
            Settings.save();
            return true;
        }

        private void discardToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // send delete file messages for each file
            for (int i = 0; i < directoryListView.SelectedIndices.Count; ++i)
            {
                string filename = directoryListView.Items[directoryListView.SelectedIndices[i]].Text;
                connectionManager.deleteFile(filename);
            }
        }

        private void enableCorrectControls()
        {
            bool an_item_is_selected = directoryListView.SelectedIndices.Count > 0;
            downloadToolStripMenuItem.Enabled = an_item_is_selected;
            discardToolStripMenuItem.Enabled = an_item_is_selected;
        }

        private void directoryListView_SelectedIndexChanged(object sender, EventArgs e)
        {
            enableCorrectControls();
        }

        private void shadowMinimap_positionChosen()
        {
            sendIntendedMotorPositions();
        }

        private void sendIntendedMotorPositions()
        {
            connectionManager.moveMotors(new long[] { orbitSliderA.Position, orbitSliderB.Position, shadowMinimap.Position.X, shadowMinimap.Position.Y, liftSliderZ.Position });
        }

        private void liftSliderZ_positionChosen()
        {
            sendIntendedMotorPositions();
        }

        private void orbitSliderA_positionChosen()
        {
            sendIntendedMotorPositions();
        }

        private void orbitSliderB_positionChosen()
        {
            sendIntendedMotorPositions();
        }
    }
}

