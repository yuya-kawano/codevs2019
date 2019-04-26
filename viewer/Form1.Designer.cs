namespace viewer
{
	partial class Form1
	{
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		/// <param name="disposing">マネージド リソースを破棄する場合は true を指定し、その他の場合は false を指定します。</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows フォーム デザイナーで生成されたコード

		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		private void InitializeComponent()
		{
			this.pictureBox = new System.Windows.Forms.PictureBox();
			this.listBoxChain = new System.Windows.Forms.ListBox();
			this.listBoxList = new System.Windows.Forms.ListBox();
			this.buttonPrev = new System.Windows.Forms.Button();
			this.buttonNext = new System.Windows.Forms.Button();
			((System.ComponentModel.ISupportInitialize)(this.pictureBox)).BeginInit();
			this.SuspendLayout();
			// 
			// pictureBox
			// 
			this.pictureBox.BackColor = System.Drawing.Color.White;
			this.pictureBox.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBox.Location = new System.Drawing.Point(12, 12);
			this.pictureBox.Name = "pictureBox";
			this.pictureBox.Size = new System.Drawing.Size(546, 707);
			this.pictureBox.TabIndex = 0;
			this.pictureBox.TabStop = false;
			// 
			// listBoxChain
			// 
			this.listBoxChain.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.listBoxChain.FormattingEnabled = true;
			this.listBoxChain.ItemHeight = 14;
			this.listBoxChain.Location = new System.Drawing.Point(564, 12);
			this.listBoxChain.Name = "listBoxChain";
			this.listBoxChain.Size = new System.Drawing.Size(108, 522);
			this.listBoxChain.TabIndex = 1;
			this.listBoxChain.SelectedIndexChanged += new System.EventHandler(this.ListBoxChain_SelectedIndexChanged);
			// 
			// listBoxList
			// 
			this.listBoxList.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
			this.listBoxList.FormattingEnabled = true;
			this.listBoxList.ItemHeight = 14;
			this.listBoxList.Location = new System.Drawing.Point(678, 12);
			this.listBoxList.Name = "listBoxList";
			this.listBoxList.Size = new System.Drawing.Size(174, 522);
			this.listBoxList.TabIndex = 2;
			this.listBoxList.SelectedIndexChanged += new System.EventHandler(this.ListBoxList_SelectedIndexChanged);
			// 
			// buttonPrev
			// 
			this.buttonPrev.Location = new System.Drawing.Point(564, 573);
			this.buttonPrev.Name = "buttonPrev";
			this.buttonPrev.Size = new System.Drawing.Size(68, 44);
			this.buttonPrev.TabIndex = 3;
			this.buttonPrev.Text = "<";
			this.buttonPrev.UseVisualStyleBackColor = true;
			this.buttonPrev.Click += new System.EventHandler(this.ButtonPrev_Click);
			// 
			// buttonNext
			// 
			this.buttonNext.Location = new System.Drawing.Point(638, 573);
			this.buttonNext.Name = "buttonNext";
			this.buttonNext.Size = new System.Drawing.Size(68, 44);
			this.buttonNext.TabIndex = 4;
			this.buttonNext.Text = ">";
			this.buttonNext.UseVisualStyleBackColor = true;
			this.buttonNext.Click += new System.EventHandler(this.ButtonNext_Click);
			// 
			// Form1
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(928, 726);
			this.Controls.Add(this.buttonNext);
			this.Controls.Add(this.buttonPrev);
			this.Controls.Add(this.listBoxList);
			this.Controls.Add(this.listBoxChain);
			this.Controls.Add(this.pictureBox);
			this.Name = "Form1";
			this.Text = "Form1";
			this.Load += new System.EventHandler(this.Form1_Load);
			((System.ComponentModel.ISupportInitialize)(this.pictureBox)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.PictureBox pictureBox;
		private System.Windows.Forms.ListBox listBoxChain;
		private System.Windows.Forms.ListBox listBoxList;
		private System.Windows.Forms.Button buttonPrev;
		private System.Windows.Forms.Button buttonNext;
	}
}

