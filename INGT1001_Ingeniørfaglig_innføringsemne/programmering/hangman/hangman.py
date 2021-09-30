from random import randint
from pathlib import Path

# This spaghetti code was written during a lecture, but it came out surprisingly
# entertaining. The dictionary contains over 60 000 norwegian words as
# line separated strings. Ascii art is shamelessly stolen from the internet.

p = Path(__file__).with_name("words.norwegian.txt")
with p.open("r") as f:
    dictionary = f.read().split()


MAX_TRIES = 5

menu_gfx = """
888   |      e      888b    |  e88~~\       e    e           e      888b    | 
888___|     d8b     |Y88b   | d888         d8b  d8b         d8b     |Y88b   | 
888   |    /Y88b    | Y88b  | 8888 __     d888bdY88b       /Y88b    | Y88b  | 
888   |   /  Y88b   |  Y88b | 8888   |   / Y88Y Y888b     /  Y88b   |  Y88b | 
888   |  /____Y88b  |   Y88b| Y888   |  /   YY   Y888b   /____Y88b  |   Y88b| 
888   | /      Y88b |    Y888  "88__/  /          Y888b /      Y88b |    Y888
"""

def random_word():
    num = randint(0, len(dictionary)-1)
    word = dictionary[num]
    return word


def get_input(prompt="", cast=None, condition=None, error_message=None):
    while True:
        try:
            response = (cast or str)(input(prompt))
            assert condition is None or condition(response)
            return response
        except:
            print(error_message or "Invalid input, try again.")


def draw_gallows(tries):
    tries_used = MAX_TRIES - tries
    gallow = '''
         _______________,,.
        /_____________.;;'/|
       |"____  _______;;;]/
       | || //'         ;
       | ||//'          ;
       | |//'           ;
       |  /'            $
       | ||             $
       | ||             $
       | ||            ,^.
       | ||            | |
       | ||            `.'
       | || 
       | || 
       | || 
       | || 
       | ||   _________________________   
       | ||  /                        4
       | || /                        /| 
       | ||/           _____        / /
       | ||           /|___/       / /|
       | ||          / /  /       / /||
       |_|/         / /  /       / / ||
      /             """""       / /| ||
     /                         / / | ||
    /                         / /  | ||
    |"""""""""""""""""""""""""|/   | ||
    |_________________________f|   | |
    | ||                    | ||           
    | ||                    | ||    
    | ||                    | ||
    | ||                    | ||
    | ||                    | ||
    | |                     | |'''

    if tries_used == 0:
        g = '''
        ____                    ___
        | ||                    | ||           
        | ||                    | ||    
        | ||                    | ||
        | ||                    | ||
        | ||                    | ||
        | |                     | |'''
        print(g)

    if tries_used == 1:
        g = '''
        |"""""""""""""""""""""""""|/
        |_________________________f
        | ||                    | ||           
        | ||                    | ||    
        | ||                    | ||
        | ||                    | ||
        | ||                    | ||
        | |                     | |'''
        print(g)

    if tries_used == 2:
        g = '''
                 /_________________________
                /                        /| 
               /           _____        / /
              /           /|___/       / /|
             /           / /  /       / /||
           /            / /  /       / / ||
          /             """""       / /| ||
         /                         / / | ||
        /                         / /  | ||
        |"""""""""""""""""""""""""|/   | ||
        |_________________________f|   | |
        | ||                    | ||           
        | ||                    | ||    
        | ||                    | ||
        | ||                    | ||
        | ||                    | ||
        | |                     | |'''
        print(g)

    if tries_used == 3:
        g = '''
           |" 
           | ||
           | ||
           | ||
           | || 
           | ||
           | ||
           | ||
           | ||
           | ||
           | || 
           | || 
           | || 
           | || 
           | ||   _________________________   
           | ||  /                        4
           | || /                        /| 
           | ||/           _____        / /
           | ||           /|___/       / /|
           | ||          / /  /       / /||
           |_|/         / /  /       / / ||
          /             """""       / /| ||
         /                         / / | ||
        /                         / /  | ||
        |"""""""""""""""""""""""""|/   | ||
        |_________________________f|   | |
        | ||                    | ||           
        | ||                    | ||    
        | ||                    | ||
        | ||                    | ||
        | ||                    | ||
        | |                     | |'''
        print(g)

    if tries_used == 4:
        g = '''
             _______________,,.
            /_____________.;;'/|
           |"____  _______;;;]/
           | || //'
           | ||//'
           | |//'
           |  /'
           | ||
           | ||
           | ||
           | ||
           | ||
           | ||
           | || 
           | || 
           | || 
           | ||   _________________________   
           | ||  /                        4
           | || /                        /| 
           | ||/           _____        / /
           | ||           /|___/       / /|
           | ||          / /  /       / /||
           |_|/         / /  /       / / ||
          /             """""       / /| ||
         /                         / / | ||
        /                         / /  | ||
        |"""""""""""""""""""""""""|/   | ||
        |_________________________f|   | |
        | ||                    | ||           
        | ||                    | ||    
        | ||                    | ||
        | ||                    | ||
        | ||                    | ||
        | |                     | |'''
        print(g)

    if tries_used == 5:
        print(gallow)


def find_all_enumerate(s, c):
    return [i for i, x in enumerate(s) if x == c]


def main():
    word = random_word()
    guessed_letters = []
    print_word = len(word) * "_"
    tries = MAX_TRIES

    print(menu_gfx)
    print(f"Velkommen til Hangman. Du har {str(MAX_TRIES)} sjanser. (Skriv 0 for å avslutte)")
    print("Ordet du skal gjette er: " + " ".join(print_word))

    while True:
        if tries <= 0:
            print("Du TAPTE!\n")
            print("Svaret var " + 14*" " + " ".join(word))
            exit()
        if print_word.count("_") == 0:
            print("DU VANT, GRATULERER!")
            exit()
        guess = get_input(prompt="Gjett ein bokstav: ",
                          cast=str,
                          condition=lambda s: len(s) == 1,
                          error_message="Ugyldig input, må vere ein bokstav")
        if guess == "0":
            exit()
        if guess not in word:
            print("Feil!")
            tries -= 1
            draw_gallows(tries)
            print(25*" " + " ".join(print_word))
            word.replace(guess, "")

        if guess in guessed_letters:
            print("Allerede gjettet!")
            tries -= 1
            draw_gallows(tries)
            print(25*" " + " ".join(print_word))
            word.replace(guess, "")

        elif guess in word:
            print("Korrekt!")
            replace_positions = find_all_enumerate(word, guess)
            for pos in replace_positions:
                print_word = print_word[0:pos] + guess + print_word[pos+1:]
            print(25*" " + " ".join(print_word))

        guessed_letters.append(guess)


if __name__ == "__main__":
    main()
