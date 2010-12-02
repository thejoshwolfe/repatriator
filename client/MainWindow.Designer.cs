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
            this.directoryImageList = new System.Windows.Forms.ImageList(this.components);
            this.downlaodAllButton = new System.Windows.Forms.Button();
            this.liftSliderZ = new System.Windows.Forms.Panel();
            this.butterflyElemtnHost = new System.Windows.Forms.Integration.ElementHost();
            this.lifeSliderZ = new System.Windows.Forms.TrackBar();
            this.miniMapPictureBox = new System.Windows.Forms.PictureBox();
            this.butterflySliderX = new System.Windows.Forms.TrackBar();
            this.butterflySliderY = new System.Windows.Forms.TrackBar();
            this.miniMapSliderX = new System.Windows.Forms.TrackBar();
            this.miniMapSliderY = new System.Windows.Forms.TrackBar();
            this.panel3 = new System.Windows.Forms.Panel();
            ((System.ComponentModel.ISupportInitialize)(this.liveViewPictureBox)).BeginInit();
            this.panel1.SuspendLayout();
            this.liftSliderZ.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.lifeSliderZ)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapPictureBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.butterflySliderX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.butterflySliderY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapSliderX)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapSliderY)).BeginInit();
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
            this.directoryListView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.directoryListView.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem1,
            listViewItem2});
            this.directoryListView.LargeImageList = this.directoryImageList;
            this.directoryListView.Location = new System.Drawing.Point(0, 0);
            this.directoryListView.Name = "directoryListView";
            this.directoryListView.Size = new System.Drawing.Size(427, 100);
            this.directoryListView.TabIndex = 1;
            this.directoryListView.UseCompatibleStateImageBehavior = false;
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
            this.liftSliderZ.Controls.Add(this.butterflyElemtnHost);
            this.liftSliderZ.Controls.Add(this.takePictureButton);
            this.liftSliderZ.Controls.Add(this.lifeSliderZ);
            this.liftSliderZ.Controls.Add(this.miniMapPictureBox);
            this.liftSliderZ.Controls.Add(this.butterflySliderX);
            this.liftSliderZ.Controls.Add(this.butterflySliderY);
            this.liftSliderZ.Controls.Add(this.miniMapSliderX);
            this.liftSliderZ.Controls.Add(this.miniMapSliderY);
            this.liftSliderZ.Dock = System.Windows.Forms.DockStyle.Right;
            this.liftSliderZ.Location = new System.Drawing.Point(427, 0);
            this.liftSliderZ.Name = "liftSliderZ";
            this.liftSliderZ.Size = new System.Drawing.Size(117, 362);
            this.liftSliderZ.TabIndex = 11;
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
            // miniMapPictureBox
            // 
            this.miniMapPictureBox.Location = new System.Drawing.Point(26, 28);
            this.miniMapPictureBox.Name = "miniMapPictureBox";
            this.miniMapPictureBox.Size = new System.Drawing.Size(79, 68);
            this.miniMapPictureBox.TabIndex = 6;
            this.miniMapPictureBox.TabStop = false;
            // 
            // butterflySliderX
            // 
            this.butterflySliderX.LargeChange = 0;
            this.butterflySliderX.Location = new System.Drawing.Point(26, 102);
            this.butterflySliderX.Maximum = 360;
            this.butterflySliderX.Name = "butterflySliderX";
            this.butterflySliderX.Size = new System.Drawing.Size(79, 45);
            this.butterflySliderX.TabIndex = 5;
            this.butterflySliderX.TickStyle = System.Windows.Forms.TickStyle.None;
            this.butterflySliderX.Value = 180;
            // 
            // butterflySliderY
            // 
            this.butterflySliderY.LargeChange = 0;
            this.butterflySliderY.Location = new System.Drawing.Point(3, 127);
            this.butterflySliderY.Maximum = 360;
            this.butterflySliderY.Name = "butterflySliderY";
            this.butterflySliderY.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.butterflySliderY.Size = new System.Drawing.Size(45, 68);
            this.butterflySliderY.TabIndex = 4;
            this.butterflySliderY.TickStyle = System.Windows.Forms.TickStyle.None;
            this.butterflySliderY.Value = 180;
            // 
            // miniMapSliderX
            // 
            this.miniMapSliderX.LargeChange = 0;
            this.miniMapSliderX.Location = new System.Drawing.Point(26, 3);
            this.miniMapSliderX.Name = "miniMapSliderX";
            this.miniMapSliderX.Size = new System.Drawing.Size(79, 45);
            this.miniMapSliderX.TabIndex = 2;
            this.miniMapSliderX.TickStyle = System.Windows.Forms.TickStyle.None;
            // 
            // miniMapSliderY
            // 
            this.miniMapSliderY.LargeChange = 0;
            this.miniMapSliderY.Location = new System.Drawing.Point(3, 28);
            this.miniMapSliderY.Name = "miniMapSliderY";
            this.miniMapSliderY.Orientation = System.Windows.Forms.Orientation.Vertical;
            this.miniMapSliderY.Size = new System.Drawing.Size(45, 68);
            this.miniMapSliderY.TabIndex = 1;
            this.miniMapSliderY.TickStyle = System.Windows.Forms.TickStyle.None;
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
            this.Load += new System.EventHandler(this.MainWindow_Load);
            ((System.ComponentModel.ISupportInitialize)(this.liveViewPictureBox)).EndInit();
            this.panel1.ResumeLayout(false);
            this.liftSliderZ.ResumeLayout(false);
            this.liftSliderZ.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.lifeSliderZ)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapPictureBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.butterflySliderX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.butterflySliderY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapSliderX)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.miniMapSliderY)).EndInit();
            this.panel3.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PictureBox liveViewPictureBox;
        private System.Windows.Forms.Button takePictureButton;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button downlaodAllButton;
        private System.Windows.Forms.Panel liftSliderZ;
        private System.Windows.Forms.TrackBar miniMapSliderY;
        private System.Windows.Forms.TrackBar miniMapSliderX;
        private System.Windows.Forms.Integration.ElementHost butterflyElemtnHost;
        private System.Windows.Forms.TrackBar butterflySliderX;
        private System.Windows.Forms.TrackBar butterflySliderY;
        private System.Windows.Forms.PictureBox miniMapPictureBox;
        private System.Windows.Forms.TrackBar lifeSliderZ;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.ListView directoryListView;
        private System.Windows.Forms.ImageList directoryImageList;

    }
}