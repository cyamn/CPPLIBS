#include <vector>
#include <cmath>
#include <ostream>
#include <limits.h>
#include <iomanip>
#include <string>
#include <iostream>

unsigned long long upperBound = 10000000000000000;
int bigDecimalLen = 16;

class bigDecimal{
  private:
    std::vector<unsigned long long> dec;
    bool negative = false;

    // remove leading cells with zeros
    void clean() {
      while (this->dec[this->dec.size()-1] == 0 && this->dec.size() > 1) {
        this->dec.pop_back();
      }
      // -0 := 0
      if (this->dec.size()==1 && this->dec[0] == 0) {
        this->negative = false;
      }
    }

  public:
    //split string in reverse into chunks and store them
    bigDecimal(std::string number) {
      // check if string starts with "-" or "+",...
      if(number.front() == '-') {
        this->negative = true;
        number = number.substr(1);
      }else if(number.front() == '+'){
        this->negative = false;
        number = number.substr(1);
      }
      int border = number.length();
      int elems = border / bigDecimalLen;
      unsigned long long num;
      int i = 1;
      while(i <= elems) {
        num = std::stoull(number.substr(border-i*bigDecimalLen,bigDecimalLen));
        dec.push_back(num);
        i+=1;
      }
    }

    bigDecimal(long long number) {
      unsigned long long offset, sum, num;
      num = (number<0)?-number:number;
      this->negative=number<0;
      if(num > upperBound) {
        offset = num / upperBound;
        sum = num - offset * upperBound;
        dec.push_back(sum);
        dec.push_back(offset);
      } else {
        dec.push_back(num);
      }
    }

    bigDecimal(std::vector<unsigned long long> dec, bool negative = false) {
      this->dec = dec;
      this->negative= negative;
    }

    bigDecimal clone() {
      return bigDecimal(this->dec);
    }

    unsigned long long get(unsigned long long index) {
      return (index > this->length()-1)?0:this->dec[index];
    }

    bigDecimal operator + (bigDecimal& b) {
      // variable declaration
      bigDecimal c = bigDecimal(this->negative);
      long long lA, lB, length, a_part, b_part, sum;
      int offset = 0;

      // calculate target length
      lA = this->length();
      lB = b.length();
      length = (lA > lB)? lA: lB;

      // sum a and b up by summing a's and b's parts and
      // take away an offset
      for(int i = 0; i < length; i++) {
        a_part = this->get(i);
        b_part = (this->negative == b.isNegative())?b.get(i):-b.get(i);
        sum = a_part+b_part+offset;
        if(sum > upperBound) {
          offset = sum /  upperBound;
          sum -= offset * upperBound;
        } else if(sum < 0){
          sum = upperBound + sum;
          offset = -1;
        } else {
          offset = 0;
        }
        c.push_back(sum);
      }

      // make target bigger if necessary
      if (offset > 0) {
        c.push_back(offset);
      } else if (offset < 0){
        c.push_back(offset);
        c.setNegative(!c.isNegative());
      }
      c.clean();
      return c;
    }
    
    bigDecimal operator - (bigDecimal& b) {
      bigDecimal minusB(b.dec, !b.negative);
      return *this + minusB;
    }

    /*
    *   This method returns a shifted big decimal by an index and a chunk and will be used for
    *   multiplikation. e.g: (1234).shift(1) -> (12340)
    *                        (1234).shift(-1)->  (1234)
    */
    bigDecimal shift(long long index, long long chunks = 0) {
      std::vector<unsigned long long> newdec;
      // convert index overshoot or undershoot to chunks
      chunks += index/bigDecimalLen;
      index %= bigDecimalLen;
      // this is used so that the index is allways positive (less cases to deal with)
      if(index < 0) {
        index = bigDecimalLen+index;
        chunks--;
      }
      //std::cout << "chunks: " << chunks << "    index: " << index << std::endl;
      // padd the chunks by filling it with zeros if chunks is positive
      for(int i = 0; i < chunks; i++) {
        newdec.push_back(0);
      }
      // padd index
      // get old and init new vars
      std::vector<unsigned long long> olddec = this->dec;
      unsigned long long current;
      // the offset will be used to skip outshifted chunks
      unsigned long long offset = (chunks<0)?-chunks:0;
      if (index == 0) {
        for(int i = offset; i < olddec.size(); i++) {
          newdec.push_back(olddec[i]);
        }
      } else if (index > 0) {
        // faktorA is a mask to extract the |index| last digits
        // faktorB is a mask to extract the rest first digits
        unsigned long long faktorA = std::pow(10,bigDecimalLen-index);
        unsigned long long faktorB = std::pow(10,index);
        // partL is will be put into next chunk
        unsigned long long partL, partR;
        partL=(offset>0)?olddec[offset-1]/faktorA:0;
        for(int i = offset; i < olddec.size(); i++) {
          current = olddec[i];
          partR = (current%faktorA)*faktorB+partL;
          partL = current / (faktorA);
          //std::cout << current << " -> " << partR << std::endl;
          newdec.push_back(partR);
        }
        if (offset < olddec.size() && partL != 0) {
          newdec.push_back(partL);
        }
      }
      // empty ^= 0
      if (newdec.size()==0) {
        newdec.push_back(0);
      }
      return bigDecimal(newdec, this->negative);
    }

    // helper mehods
    void push_back(unsigned long long num){
        this->dec.push_back(num);
    }
    unsigned long long length() {
      return dec.size();
    }

    bool isNegative() {
      return negative;
    }

    void setNegative(bool negative) {
      this->negative = negative;
    }

};

// this makes bigDecimal printable
std::ostream & operator<<(std::ostream &os, bigDecimal &b) {
      if(b.isNegative()) os << "-";
      os << b.get(b.length() - 1);
      for (int i = b.length() - 2; i >= 0; i--) {
        os << std::setfill('0') << std::setw(bigDecimalLen) << b.get(i);
      }
      return os;
}