from tkinter import *



class Window(Frame):

	def __init__(self, master=None):
		Frame.__init__(self, master)               
		self.master = master
		#self.init_window()

	#def init_window(self):
		# changing the title of our master widget      
		self.master.title("LitterRate Desktop Application")

		# allowing the widget to take the full space of the root window
		self.pack(fill=BOTH, expand=1)

		# creating a button instance
		self.quitButton = Button(self, text="Quit", command=self.client_exit)
		
		# creating a button instance
		self.getDistDataButton = Button(self, text="Get New Reading", command=self.fakeTextUpdate1)
		
		# creating a button instance
		self.getLightDataButton = Button(self, text="Get Most Recent Reading", command=self.fakeTextUpdate2)
		
		#creating the text output area
		self.dataSectionHeader=Label(self, text = "Data Reading")
		self.dataSectionHeader.place(x=50,y=100)
		self.timeLabel=Label(self,text="Time: ")
		self.timeLabel.place(x=50,y=120)
		self.readingLabel=Label(self,text="Reading: ")
		self.readingLabel.place(x=50,y=140)
		self.valueLabel=Label(self,text="Value: ")
		self.valueLabel.place(x=50,y=160)
		
		
		# placing the button on my window
		self.quitButton.place(x=300, y=250)
		
		self.getDistDataButton.place (x = 0, y =0)
		self.getLightDataButton.place(x = 250, y =0)
		
		#self.w = Label(self, text="Hello, world!")
		#self.w.pack()
		
		
		
	def client_exit(self):
			exit()
	def fakeTextUpdate1(self):
			self.timeLabel.config(text="Time: Some Fake Value")
			self.readingLabel.config(text="Reading: Some Fake Integer")
			self.valueLabel.config(text="Value: Bin is fake")
	def fakeTextUpdate2(self):
			self.timeLabel.config(text="Time: Some Other Fake Value")
			self.readingLabel.config(text="Reading: Some Other Fake Integer")
			self.valueLabel.config(text="Value: Bin is really fake")
		
root = Tk()
root.geometry("400x300")
app = Window(root)
root.mainloop()