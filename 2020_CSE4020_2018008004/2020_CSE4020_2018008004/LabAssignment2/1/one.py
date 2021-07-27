import numpy as np

def main():
    # A
    M = np.array([i for i in range(2,27)])
    print (M)
    print ()
    # B
    M = M.reshape(5, 5)
    print (M)
    print ()
    # C
    M[1:4, 1:4] = np.zeros(9).reshape(3, 3)
    print (M)
    print ()
    # D
    M = M @ M
    print (M)
    print ()
    # E
    print (np.sqrt(np.sum(M[0]*M[0])))
    print ()

# Main
if __name__ == "__main__":
    main()
