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
            System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem("asdf", 0);
            System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem("fdsa", 0);
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
            this.liftSliderZ = new System.Windows.Forms.Panel();
            this.shadowSlider1 = new repatriator_client.ShadowSlider();
            this.butterflyElemtnHost = new System.Windows.Forms.Integration.ElementHost();
            this.lifeSliderZ = new System.Windows.Forms.TrackBar();
            this.panel3 = new System.Windows.Forms.Panel();
            this.shadowMinimap1 = new repatriator_client.ShadowMinimap();
            this.shadowSlider2 = new repatriator_client.ShadowSlider();
            ((System.ComponentModel.ISupportInitialize)(this.liveViewPictureBox)).BeginInit();
            this.panel1.SuspendLayout();
            this.directoryContextMenu.SuspendLayout();
            this.liftSliderZ.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.lifeSliderZ)).BeginInit();
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
            this.takePictureButton.Location = new System.Drawing.Point(34, 201);
            this.takePictureButton.Name = "takePictureButton";
            this.takePictureButton.Size = new System.Drawing.Size(71, 156);
            this.takePictureButton.TabIndex = 9;
            this.takePictureButton.Text = "Take Picture";
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
            this.directoryListView.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem1,
            listViewItem2});
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
            // liftSliderZ
            // 
            this.liftSliderZ.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(128)))), ((int)(((byte)(128)))), ((int)(((byte)(255)))));
            this.liftSliderZ.Controls.Add(this.shadowSlider2);
            this.liftSliderZ.Controls.Add(this.shadowMinimap1);
            this.liftSliderZ.Controls.Add(this.shadowSlider1);
            this.liftSliderZ.Controls.Add(this.butterflyElemtnHost);
            this.liftSliderZ.Controls.Add(this.takePictureButton);
            this.liftSliderZ.Controls.Add(this.lifeSliderZ);
            this.liftSliderZ.Dock = System.Windows.Forms.DockStyle.Right;
            this.liftSliderZ.Location = new System.Drawing.Point(427, 0);
            this.liftSliderZ.Name = "liftSliderZ";
            this.liftSliderZ.Size = new System.Drawing.Size(117, 362);
            this.liftSliderZ.TabIndex = 11;
            // 
            // shadowSlider1
            // 
            this.shadowSlider1.Location = new System.Drawing.Point(3, 127);
            this.shadowSlider1.MaxPosition = 100;
            this.shadowSlider1.Name = "shadowSlider1";
            this.shadowSlider1.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.shadowSlider1.Position = 80;
            this.shadowSlider1.ShadowPosition = 20;
            this.shadowSlider1.Size = new System.Drawing.Size(17, 68);
            this.shadowSlider1.TabIndex = 10;
            this.shadowSlider1.Text = "shadowSlider1";
            // 
            // butterflyElemtnHost
            // 
            this.butterflyElemtnHost.Location = new System.Drawing.Point(26, 127);
            this.butterflyElemtnHost.Name = "butterflyElemtnHost";
            this.butterflyElemtnHost.Size = new System.Drawing.Size(79, 68);
            this.butterflyElemtnHost.TabIndex = 3;
            this.butterflyElemtnHost.Text = "elementHost1";
            this.butterflyElemtnHost.Child = null;
            // 
            // lifeSliderZ
            // 
            this.lifeSliderZ.LargeChange = 0;
            this.lifeSliderZ.Location = new System.Drawing.Point(3, 201);
            this.lifeSliderZ.Name = "lifeSliderZ";
            this.lifeSliderZ.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.lifeSliderZ.Size = new System.Drawing.Size(45, 156);
            this.lifeSliderZ.TabIndex = 7;
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
            // shadowMinimap1
            // 
            this.shadowMinimap1.Location = new System.Drawing.Point(26, 12);
            this.shadowMinimap1.MaxPosition = new System.Drawing.Point(100, 100);
            this.shadowMinimap1.Name = "shadowMinimap1";
            this.shadowMinimap1.Position = new System.Drawing.Point(80, 80);
            this.shadowMinimap1.ShadowPosition = new System.Drawing.Point(20, 20);
            this.shadowMinimap1.Size = new System.Drawing.Size(79, 68);
            this.shadowMinimap1.TabIndex = 11;
            this.shadowMinimap1.Text = "shadowMinimap1";
            // 
            // shadowSlider2
            // 
            this.shadowSlider2.Location = new System.Drawing.Point(26, 104);
            this.shadowSlider2.MaxPosition = 100;
            this.shadowSlider2.Name = "shadowSlider2";
            this.shadowSlider2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            this.shadowSlider2.Position = 80;
            this.shadowSlider2.ShadowPosition = 20;
            this.shadowSlider2.Size = new System.Drawing.Size(79, 17);
            this.shadowSlider2.TabIndex = 12;
            this.shadowSlider2.Text = "shadowSlider2";
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(544, 462);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.liftSliderZ);
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
            this.liftSliderZ.ResumeLayout(false);
            this.liftSliderZ.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.lifeSliderZ)).EndInit();
            this.panel3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PictureBox liveViewPictureBox;
        private System.Windows.Forms.Button takePictureButton;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button downlaodAllButton;
        private System.Windows.Forms.Panel liftSliderZ;
        private System.Windows.Forms.Integration.ElementHost butterflyElemtnHost;
        private System.Windows.Forms.TrackBar lifeSliderZ;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.ListView directoryListView;
        private System.Windows.Forms.ImageList directoryImageList;
        private System.Windows.Forms.ContextMenuStrip directoryContextMenu;
        private System.Windows.Forms.ToolStripMenuItem downloadToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem discardToolStripMenuItem;
        private ShadowSlider shadowSlider1;
        private ShadowSlider shadowSlider2;
        private ShadowMinimap shadowMinimap1;

    }
}