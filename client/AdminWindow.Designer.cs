namespace repatriator_client
{
    partial class AdminWindow
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
            System.Windows.Forms.Label label4;
            System.Windows.Forms.Label label3;
            System.Windows.Forms.Label label2;
            this.passwordText = new System.Windows.Forms.TextBox();
            this.userNameText = new System.Windows.Forms.TextBox();
            this.serverText = new System.Windows.Forms.TextBox();
            this.cancelConncetionButton = new System.Windows.Forms.Button();
            this.connectButton = new System.Windows.Forms.Button();
            label4 = new System.Windows.Forms.Label();
            label3 = new System.Windows.Forms.Label();
            label2 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label4
            // 
            label4.AutoSize = true;
            label4.Location = new System.Drawing.Point(16, 67);
            label4.Name = "label4";
            label4.Size = new System.Drawing.Size(55, 13);
            label4.TabIndex = 20;
            label4.Text = "password:";
            // 
            // passwordText
            // 
            this.passwordText.Location = new System.Drawing.Point(77, 64);
            this.passwordText.Name = "passwordText";
            this.passwordText.Size = new System.Drawing.Size(157, 20);
            this.passwordText.TabIndex = 19;
            this.passwordText.Text = "temp1234";
            this.passwordText.UseSystemPasswordChar = true;
            // 
            // label3
            // 
            label3.AutoSize = true;
            label3.Location = new System.Drawing.Point(12, 41);
            label3.Name = "label3";
            label3.Size = new System.Drawing.Size(59, 13);
            label3.TabIndex = 18;
            label3.Text = "user name:";
            // 
            // label2
            // 
            label2.AutoSize = true;
            label2.Location = new System.Drawing.Point(32, 15);
            label2.Name = "label2";
            label2.Size = new System.Drawing.Size(39, 13);
            label2.TabIndex = 17;
            label2.Text = "server:";
            // 
            // userNameText
            // 
            this.userNameText.Location = new System.Drawing.Point(77, 38);
            this.userNameText.Name = "userNameText";
            this.userNameText.Size = new System.Drawing.Size(157, 20);
            this.userNameText.TabIndex = 16;
            this.userNameText.Text = "default_admin";
            // 
            // serverText
            // 
            this.serverText.Location = new System.Drawing.Point(77, 12);
            this.serverText.Name = "serverText";
            this.serverText.Size = new System.Drawing.Size(157, 20);
            this.serverText.TabIndex = 15;
            this.serverText.Text = "localhost:57051";
            // 
            // cancelConncetionButton
            // 
            this.cancelConncetionButton.Location = new System.Drawing.Point(159, 90);
            this.cancelConncetionButton.Name = "cancelConncetionButton";
            this.cancelConncetionButton.Size = new System.Drawing.Size(75, 23);
            this.cancelConncetionButton.TabIndex = 22;
            this.cancelConncetionButton.Text = "Cancel";
            this.cancelConncetionButton.UseVisualStyleBackColor = true;
            // 
            // connectButton
            // 
            this.connectButton.Location = new System.Drawing.Point(77, 90);
            this.connectButton.Name = "connectButton";
            this.connectButton.Size = new System.Drawing.Size(76, 23);
            this.connectButton.TabIndex = 21;
            this.connectButton.Text = "Connect";
            this.connectButton.UseVisualStyleBackColor = true;
            this.connectButton.Click += new System.EventHandler(this.connectButton_Click);
            // 
            // AdminWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.cancelConncetionButton);
            this.Controls.Add(this.connectButton);
            this.Controls.Add(label4);
            this.Controls.Add(this.passwordText);
            this.Controls.Add(label3);
            this.Controls.Add(label2);
            this.Controls.Add(this.userNameText);
            this.Controls.Add(this.serverText);
            this.Name = "AdminWindow";
            this.Text = "Repatriator Admin";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.AdminWindow_FormClosed);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox passwordText;
        private System.Windows.Forms.TextBox userNameText;
        private System.Windows.Forms.TextBox serverText;
        private System.Windows.Forms.Button cancelConncetionButton;
        private System.Windows.Forms.Button connectButton;
    }
}