#include <string.h>
#include <iostream>
#include <fstream>

using namespace std;

class WrongDim{};
class IndexOutOfRange{};
class FileDoesntExist{};

class matrix{
      struct figure;
      figure *data;
      class row;
      
      public:
             matrix(unsigned int, unsigned int, double, double);
             matrix( const matrix &);
             matrix(fstream &);
             ~matrix();
             matrix & operator=(const matrix &);
	     matrix operator*(const matrix &) const;
	     row operator[](unsigned int);
             friend ostream & operator<<(ostream & , matrix &);
             double** alloc(unsigned int, unsigned int);
             void free(unsigned int, double**);
};

struct matrix::figure{
       unsigned int counter;
       double **values;
       unsigned int row;
       unsigned int col;
            
       figure(unsigned int nrow, unsigned int ncol, double **v)
       {
                       unsigned int i,j;
                       row = nrow;
                       col = ncol;
		       counter = 1;
			
                       values = new double*[row];

                       for(i=0;i<row;i++)
                       {
					try{
                                         values[i] = new double[col];}
					catch(bad_alloc&)
					{
						for(unsigned int g = 0; g<i; g++)
							delete [] values[g];
						delete [] values;
						throw;
					}
                       }
                       
                       for(i=0;i<row;i++)
                       {
                                         for(j=0;j<col;j++)
                                         {
                                                           values[i][j]=v[i][j];
                                         }
                       }
       }
       
       ~figure()
       {
                for(unsigned int i=0;i<row;i++)
                        delete [] values[i];
                delete [] values;
       }
       
       figure* detach()
       {
               if(counter == 1)
               return this;
               
               figure *nfigure = new figure(row,col,values);
               counter--;
               return nfigure;
       }
       
};

class matrix::row
{
      friend class matrix;
      class element;
      matrix &r;
      unsigned int i;
      
      row (matrix & rr, unsigned int ii): r(rr), i(ii) {};
      
      public:
             element operator[](unsigned int);
             double read(unsigned int) const;
             void write(unsigned int, double);         
};

matrix::row  matrix::operator[](unsigned int i)
{
     if(i<data->row)
     return row(*this,i);
     else
     throw IndexOutOfRange(); 
}

double matrix::row::read(unsigned int j) const
{
     return r.data->values[i][j];
}

void matrix::row::write(unsigned int j, double a)
{
     r.data = r.data->detach();
     r.data->values[i][j] = a;
}


class matrix::row::element
{
      friend class row;
      row &e;
      unsigned int j;
      
      element(row &ee, unsigned int jj): e(ee), j(jj) {};
      public:
             operator double() const
             {
                      return e.read(j);
             }
             
             matrix::row::element & operator=(double a)
             {
                              e.write(j,a);
                              return *this;
             }
} ;


matrix::row::element matrix::row::operator[](unsigned int j)
             {
		     if(j<r.data->col)
                     return element(*this, j);
		     else
		     throw IndexOutOfRange();
             }                     
             

matrix::matrix(unsigned int r, unsigned int c, double v1 = 0, double v2 = 0)
{
		       unsigned int i,j;
                       double **values;
			
                       values = alloc(r,c);
		       
                       for(i=0;i<r;i++)
                       {
                                         for(j=0;j<c;j++)
                                         {
                                                           values[i][j] = v2;
                                         }
                       }
                       
                       i=0;
                       j=0;
                       while(i<r&&j<c)
                       {
                                          values[i][j]=v1;
                                          i++;
                                          j++;
                       }
                        data = new figure(r,c,values);
                        
                        for(i=0;i<r;i++)
                {
                        delete [] values[i];
                }
                
                delete [] values;
}

matrix::matrix(const matrix &m)
{
    m.data->counter++;
    data=m.data;
}

matrix::matrix(fstream &f)
{
               if(f)
               {
               unsigned int i,j;
               unsigned int r;
               unsigned int c;
               double** values;
               
               f>>r;
               f>>c;
	       
               values  = alloc(r,c);
              
               
               for(i = 0; i < r; i++)
               {
                     for(j=0;j<c;j++)
                     {
                                     f>>values[i][j];
                     }
               }
               
               data = new figure(r,c,values);
               
               free(r,values);
               }
               else
                throw FileDoesntExist();
               
}
                      
matrix::~matrix()
{
  if(--data->counter==0)
    delete data;
}

matrix & matrix::operator=(const matrix &m)
{
       m.data->counter++;
       if(--data->counter==0)
       delete data;
       data = m.data;
       return *this;
}                    
                                                                                                  
                       
matrix matrix::operator*(const matrix &m) const
{
	
	if(data->col==m.data->row)
        {
                       unsigned int i,j,k;
		       matrix R(data->row, m.data->col);
                      
                       for(i=0;i<data->row;i++)
                       {
                                              for(j=0;j<m.data->col;j++)
                                              {
                                                                        for(k = 0;k<data->col;k++)
                                                                        R.data->values[i][j]+=data->values[i][k]*m.data->values[k][j];
                                              }
                       }
	return R;
	
	}
	else 
	  throw WrongDim();
}     
  
ostream & operator<<(ostream &o, matrix &m)
{
       unsigned int i,j;
        
        for(i=0;i<m.data->row;i++)
                       {
                                         for(j=0;j<m.data->col;j++)
                                         {
                                                           o<<m.data->values[i][j]<<"    ";
                                         }
                                         o<<endl;
                       }         
        return o;
}

double** matrix::alloc(unsigned int r, unsigned int c)
{
			double **values;
                       values = new double*[r];

                       for(unsigned int i=0; i<r;i++)
                       {
					try{
                                         values[i] = new double[c];}
                                         
					catch(bad_alloc&)
					{
                                    	for(unsigned int g = 0; g<i; g++)
                						delete [] values[g];
                						delete [] values;
                						throw;
					}
                }
                    return values;
}

void matrix::free(unsigned int r, double** values)
{
     for(unsigned int i=0; i<r; i++)
     delete [] values[i];
     
     delete [] values;
}
                  
int main()
{
    try{
    matrix A1(2,5,1);
    
    cout<<A1<<endl;
    
    matrix A2(5,3,0,6.3);
    
    cout << A2 << endl;
    
    matrix S = A1*A2;

    cout<<S<<endl;
    
    fstream f1;
		f1.open("matrix.dat", fstream::in);
		matrix B(f1);
    f1.close();	
    cout<<B<<endl;
    
    S = B;
    
    cout<<S<<endl;

    S[0][0] = 1.4;					
    cout << "S[0][0]=" << S[0][0] << endl;		
    cout << "B[0][0]=" << B[0][0] << endl;
}
    catch(WrongDim&)
	{
		cout << "Wrong Matrix Dimensions" << endl;
	}
    catch(IndexOutOfRange&)
	{
		cout << "Index Out of Range" << endl;
	}
	
    catch(bad_alloc&)
	{
		cout << "Out of Memory" << endl;
	}
    catch(FileDoesntExist)
	{
        cout<<"File doesn't exist"<<endl;
    }

    return 0;
}
        
                       
                       
                       
                       
                       
                       
