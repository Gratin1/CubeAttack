import sys

def GetFromFIle(PathToFile):
    with open(PathToFile) as infile:
        return list(infile)

class MaxtermMatrixEquation:
    def __init__(self, left_, right_):
        #left and right parts of equations
        self.left = left_
        self.right = right_
        self.ConvertStrToList()

    def ConvertStrToList(self):
        #e.g. from '0101' -> ['0','1','0','1']
        self.left = list(map(lambda x: x.split(' '), self.left))
        #['0','1','0','1'] -> [0,1,0,1]
        for i in range(len(self.left)):
            self.left[i] = list(map(lambda x: eval(x), self.left[i]))
        for i in range(len(self.right)):
            self.right[i] = eval(self.right[i])

    def MakeMatrix(self):
        self.Mtx = Matrix(GF(2), self.left)
        #print(self.Mtx)
    
    def SolveMatrix(self):
        return self.Mtx.solve_right(vector(self.right))


if __name__=="__main__":
    if len(sys.argv) == 2:
        raise Exception
    maxterms = GetFromFIle(sys.argv[1])
    values = GetFromFIle(sys.argv[2])
    test = MaxtermMatrixEquation(maxterms, values)
    test.MakeMatrix()
    print(test.SolveMatrix())
    print(len(sys.argv))
