namespace repatriator_client
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainWindow));
            this.liveViewPictureBox = new System.Windows.Forms.PictureBox();
            this.takePictureButton = new System.Windows.Forms.Button();
            this.panel1 = new System.Windows.Forms.Panel();
            this.directoryListView = new System.Windows.Forms.ListView();
            this.directoryContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.downloadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.discardToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.directoryImageList = new System.Windows.Forms.ImageList(this.components);
            this.downlaodAllButton = new System.Windows.Forms.Button();
            this.tinkyWinky = new System.Windows.Forms.Panel();
            this.butterflyElemtnHost = new System.Windows.Forms.Integration.ElementHost();
            this.panel3 = new System.Windows.Forms.Panel();
            this.orbitSliderB = new repatriator_client.ShadowSlider();
            this.shadowMinimap = new repatriator_client.ShadowMinimap();
            this.liftSliderZ = new repatriator_client.ShadowSlider();
            this.orbitSliderA = new repatriator_client.ShadowSlider();
            ((System.ComponentModel.ISupportInitialize)(this.liveViewPictureBox)).BeginInit();
            this.panel1.SuspendLayout();
            this.directoryContextMenu.SuspendLayout();
            this.tinkyWinky.SuspendLayout();
            this.panel3.SuspendLayout();
            this.SuspendLayout();
            // 
            // liveViewPictureBox
            // 
            this.liveViewPictureBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.liveViewPictureBox.Location = new System.Drawing.Point(0, 0);
            this.liveViewPictureBox.Name = "liveViewPictureBox";
            this.liveViewPictureBox.Size = new System.Drawing.Size(427, 362);
            this.liveViewPictureBox.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.liveViewPictureBox.TabIndex = 8;
            this.liveViewPictureBox.TabStop = false;
            // 
            // takePictureButton
            // 
            this.takePictureButton.AccessibleDescription = "Take Picture";
            this.takePictureButton.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("takePictureButton.BackgroundImage")));
            this.takePictureButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.takePictureButton.Location = new System.Drawing.Point(3, 130);
            this.takePictureButton.Name = "takePictureButton";
            this.takePictureButton.Size = new System.Drawing.Size(63, 58);
            this.takePictureButton.TabIndex = 9;
            this.takePictureButton.UseVisualStyleBackColor = true;
            this.takePictureButton.Click += new System.EventHandler(this.takePictureButton_Click);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.directoryListView);
            this.panel1.Controls.Add(this.downlaodAllButton);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 362);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(544, 100);
            this.panel1.TabIndex = 10;
            // 
            // directoryListView
            // 
            this.directoryListView.ContextMenuStrip = this.directoryContextMenu;
            this.directoryListView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.directoryListView.LargeImageList = this.directoryImageList;
            this.directoryListView.Location = new System.Drawing.Point(0, 0);
            this.directoryListView.Name = "directoryListView";
            this.directoryListView.Size = new System.Drawing.Size(427, 100);
            this.directoryListView.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.directoryListView.TabIndex = 1;
            this.directoryListView.UseCompatibleStateImageBehavior = false;
            this.directoryListView.SelectedIndexChanged += new System.EventHandler(this.directoryListView_SelectedIndexChanged);
            // 
            // directoryContextMenu
            // 
            this.directoryContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.downloadToolStripMenuItem,
            this.discardToolStripMenuItem});
            this.directoryContextMenu.Name = "directoryContextMenu";
            this.directoryContextMenu.Size = new System.Drawing.Size(129, 48);
            // 
            // downloadToolStripMenuItem
            // 
            this.downloadToolStripMenuItem.Name = "downloadToolStripMenuItem";
            this.downloadToolStripMenuItem.Size = new System.Drawing.Size(128, 22);
            this.downloadToolStripMenuItem.Text = "Do&wnload";
            this.downloadToolStripMenuItem.Click += new System.EventHandler(this.downloadToolStripMenuItem_Click);
            // 
            // discardToolStripMenuItem
            // 
            this.discardToolStripMenuItem.Name = "discardToolStripMenuItem";
            this.discardToolStripMenuItem.Size = new System.Drawing.Size(128, 22);
            this.discardToolStripMenuItem.Text = "&Discard";
            this.discardToolStripMenuItem.Click += new System.EventHandler(this.discardToolStripMenuItem_Click);
            // 
            // directoryImageList
            // 
            this.directoryImageList.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("directoryImageList.ImageStream")));
            this.directoryImageList.TransparentColor = System.Drawing.Color.Transparent;
            this.directoryImageList.Images.SetKeyName(0, "mnwla.jpg");
            // 
            // downlaodAllButton
            // 
            this.downlaodAllButton.Dock = System.Windows.Forms.DockStyle.Right;
            this.downlaodAllButton.Location = new System.Drawing.Point(427, 0);
            this.downlaodAllButton.Name = "downlaodAllButton";
            this.downlaodAllButton.Size = new System.Drawing.Size(117, 100);
            this.downlaodAllButton.TabIndex = 0;
            this.downlaodAllButton.Text = "Download All";
            this.downlaodAllButton.UseVisualStyleBackColor = true;
            // 
            // tinkyWinky
            // 
            this.tinkyWinky.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(128)))), ((int)(((byte)(255)))));
            this.tinkyWinky.Controls.Add(this.orbitSliderB);
            this.tinkyWinky.Controls.Add(this.shadowMinimap);
            this.tinkyWinky.Controls.Add(this.liftSliderZ);
            this.tinkyWinky.Controls.Add(this.orbitSliderA);
            this.tinkyWinky.Controls.Add(this.butterflyElemtnHost);
            this.tinkyWinky.Controls.Add(this.takePictureButton);
            this.tinkyWinky.Dock = System.Windows.Forms.DockStyle.Right;
            this.tinkyWinky.Location = new System.Drawing.Point(427, 0);
            this.tinkyWinky.Name = "tinkyWinky";
            this.tinkyWinky.Size = new System.Drawing.Size(117, 362);
            this.tinkyWinky.TabIndex = 11;
            // 
            // butterflyElemtnHost
            // 
            this.butterflyElemtnHost.Location = new System.Drawing.Point(26, 288);
            this.butterflyElemtnHost.Name = "butterflyElemtnHost";
            this.butterflyElemtnHost.Size = new System.Drawing.Size(79, 68);
            this.butterflyElemtnHost.TabIndex = 3;
            this.butterflyElemtnHost.Text = "elementHost1";
            this.butterflyElemtnHost.Child = null;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.liveViewPictureBox);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel3.Location = new System.Drawing.Point(0, 0);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(427, 362);
            this.panel3.TabIndex = 12;
            // 
            // orbitSliderB
            // 
            this.orbitSliderB.Location = new System.Drawing.Point(26, 265);
            this.orbitSliderB.MaxPosition = 10000;
            this.orbitSliderB.Name = "orbitSliderB";
            this.orbitSliderB.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.orbitSliderB.Position = 80;
            this.orbitSliderB.ShadowPosition = 20;
            this.orbitSliderB.Size = new System.Drawing.Size(79, 17);
            this.orbitSliderB.TabIndex = 12;
            this.orbitSliderB.Text = "shadowSlider2";
            this.orbitSliderB.positionChosen += new System.Action(this.orbitSliderB_positionChosen);
            // 
            // shadowMinimap
            // 
            this.shadowMinimap.Location = new System.Drawing.Point(6, 12);
            this.shadowMinimap.MaxPosition = new System.Drawing.Point(100, 100);
            this.shadowMinimap.Name = "shadowMinimap";
            this.shadowMinimap.Position = new System.Drawing.Point(80, 80);
            this.shadowMinimap.ShadowPosition = new System.Drawing.Point(20, 20);
            this.shadowMinimap.Size = new System.Drawing.Size(99, 68);
            this.shadowMinimap.TabIndex = 11;
            this.shadowMinimap.positionChosen += new System.Action(this.shadowMinimap_positionChosen);
            // 
            // liftSliderZ
            // 
            this.liftSliderZ.Location = new System.Drawing.Point(88, 86);
            this.liftSliderZ.MaxPosition = 500000;
            this.liftSliderZ.Name = "liftSliderZ";
            this.liftSliderZ.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.liftSliderZ.Position = 80;
            this.liftSliderZ.ShadowPosition = 20;
            this.liftSliderZ.Size = new System.Drawing.Size(17, 173);
            this.liftSliderZ.TabIndex = 10;
            this.liftSliderZ.positionChosen += new System.Action(this.liftSliderZ_positionChosen);
            // 
            // orbitSliderA
            // 
            this.orbitSliderA.Location = new System.Drawing.Point(3, 288);
            this.orbitSliderA.MaxPosition = 484000;
            this.orbitSliderA.Name = "orbitSliderA";
            this.orbitSliderA.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.orbitSliderA.Position = 80;
            this.orbitSliderA.ShadowPosition = 20;
            this.orbitSliderA.Size = new System.Drawing.Size(17, 68);
            this.orbitSliderA.TabIndex = 10;
            this.orbitSliderA.Text = "shadowSlider1";
            this.orbitSliderA.positionChosen += new System.Action(this.orbitSliderA_positionChosen);
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(544, 462);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.tinkyWinky);
            this.Controls.Add(this.panel1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(560, 500);
            this.Name = "MainWindow";
            this.Text = "Repatriator";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainWindow_FormClosing);
            this.Load += new System.EventHandler(this.MainWindow_Load);
            ((System.ComponentModel.ISupportInitialize)(this.liveViewPictureBox)).EndInit();
            this.panel1.ResumeLayout(false);
            this.directoryContextMenu.ResumeLayout(false);
            this.tinkyWinky.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PictureBox liveViewPictureBox;
        private System.Windows.Forms.Button takePictureButton;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button downlaodAllButton;
        private System.Windows.Forms.Panel tinkyWinky;
        private System.Windows.Forms.Integration.ElementHost butterflyElemtnHost;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.ListView directoryListView;
        private System.Windows.Forms.ImageList directoryImageList;
        private System.Windows.Forms.ContextMenuStrip directoryContextMenu;
        private System.Windows.Forms.ToolStripMenuItem downloadToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem discardToolStripMenuItem;
        private ShadowSlider orbitSliderA;
        private ShadowSlider orbitSliderB;
        private ShadowMinimap shadowMinimap;
        private ShadowSlider liftSliderZ;

    }
}