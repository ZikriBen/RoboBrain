import RoboBrain
import threading

def thread_function():
    rb.run()


menu_options = {
    1: 'add_net_func 1 (toBase)',
    2: 'add_net_func 2 (reverse)',
    3: 'Exit',
}

def print_menu():
    for key in menu_options.keys():
        print (key, '--', menu_options[key] )

if __name__=='__main__':
    rb = RoboBrain.RoboBrain()
    rb.load_config(r"C:\Users\user\source\repos\ConsoleApplication1\x64\Release\config.yaml")
    threading.Thread(target=thread_function).start()
    
    while(True):
        print_menu()
        option = ''
        try:
            option = int(input('Enter your choice: '))
        except:
            print('Wrong input. Please enter a number ...')
        if option == 1:
           rb.add_net_func("toBase")
        elif option == 2:
            rb.add_net_func("reverse")
        elif option == 3:
            exit()
        else:
            print('Invalid option. Please enter a number between 1 and 3.')