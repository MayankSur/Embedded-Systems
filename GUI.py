from tkinter import *



class Window(Frame):

	def __init__(self, master=None):
		Frame.__init__(self, master)               
		self.master = master
		self.init_window()

	def init_window(self):
		# changing the title of our master widget      
		self.master.title("GUI")

		# allowing the widget to take the full space of the root window
		self.pack(fill=BOTH, expand=1)

		# creating a button instance
		quitButton = Button(self, text="Quit", command=self.client_exit)
		
		# creating a button instance
		getDistDataButton = Button(self, text="Get New Reading", command=self.client_exit)
		
		# creating a button instance
		getLightDataButton = Button(self, text="Get Most Recent Reading", command=self.client_exit)
		
		#creating the text output area
		outputInfoArea = Label(self, text="test")
		outputInfoArea.place(x=100,y=200)
		#outputInfoArea.pack()
		
		# placing the button on my window
		quitButton.place(x=300, y=250)
		
		getDistDataButton.place (x = 100, y =100)
		getLightDataButton.place(x = 250, y =100)
		
		w = Label(self, text="Hello, world!")
		w.pack()
		
		
		
	def client_exit(self):
			exit()
	
		
root = Tk()
root.geometry("400x300")
app = Window(root)
root.mainloop()