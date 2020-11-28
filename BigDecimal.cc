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

  public:
    bigDecimal() {}

    //split string in reverse into chunks and store them
    bigDecimal(std::string number) {
      int border = number.length();
      int elems = border / bigDecimalLen;
      unsigned long long num;
      int i = 0;
      while(i < elems) {
        num = std::stoull(number.substr(border-(i+1)*bigDecimalLen,bigDecimalLen));
        dec.push_back(num);
        i+=1;
      }
      num = std::stoull(number.substr(0, border-i*bigDecimalLen));
      dec.push_back(num);
    }

    bigDecimal(unsigned long long num) {
      unsigned long long offset, sum;
      if(num > upperBound) {
        offset = num / upperBound;
        sum = num - offset * upperBound;
        dec.push_back(sum);
        dec.push_back(offset);
      } else {
        dec.push_back(num);
      }
    }

    bigDecimal(std::vector<unsigned long long> dec) {
      this->dec = dec;
    }

    bigDecimal clone() {
      return bigDecimal(this->dec);
    }

    unsigned long long get(unsigned long long index) {
      return (index > this->length()-1)?0:this->dec[index];
    }

    bigDecimal operator + (bigDecimal& b) {
      // variable declaration
      bigDecimal c = bigDecimal();
      unsigned long long lA, lB, length, a_part, b_part, sum;
      unsigned int offset = 0;

      // calculate target length
      lA = this->length();
      lB = b.length();
      length = (lA > lB)? lA: lB;

      // sum a and b up by summing a's and b's parts and
      // take away an offset
      for(int i = 0; i < length; i++) {
        a_part = this->get(i);
        b_part = b.get(i);
        sum = a_part+b_part+offset;
        if(sum > upperBound) {
          offset = sum /  upperBound;
          sum -= offset * upperBound;
        } else {
          offset = 0;
        }
        c.push_back(sum);
      }

      // make target bigger if necessary
      if (offset > 0) {
        c.push_back(offset);
      }
      return c;
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
      return bigDecimal(newdec);
    }

    // helper mehods
    void push_back(unsigned long long num){
        this->dec.push_back(num);
    }
    unsigned long long length() {
      return dec.size();
    }
};

// this makes bigDecimal printable
std::ostream & operator<<(std::ostream &os, bigDecimal &b) {
      os << b.get(b.length() - 1);
      for (int i = b.length() - 2; i >= 0; i--) {
        os << std::setfill('0') << std::setw(bigDecimalLen) << b.get(i);
      }
      return os;
}