
def main():
  buffer = [(-1,-1)] * 4096
  names = []
  filename = 'list_of_chords.txt'
  with open( filename ) as file:
    for line in file:
      data = line.split( ',', 2 )
      chord = data[1].split()
      if data[0][0] == '%':
        continue
      number = 0
      for note in chord:
        number += 2 ** int(note)
      names.append( (number, data[0]) )
      for i in range( 0, 12 ):
        if buffer[number] != (-1,-1):
          raise ValueError( number, (i, names[len(names)-1]), (buffer[number][0],names[buffer[number][1]-1] ) )
        buffer[number] = (i, len(names) )
        number *= 2
        if number >= 4096:
          number -= 4095
  print buffer
  print names

if __name__ == '__main__':
  main()