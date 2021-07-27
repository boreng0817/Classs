# Operating System [ELE3021-12962]

___prof. HyungSoo Jung___

___2018008004 Kim SiWoo___





## Project 3 [File System]

### Milestone1 [Expanding file size]

---

Basic xv6 inode can take 12 + 128 blocks. Each block has 512bytes, so max file size is 70KB.   

So, in project 3, we will expand maximum file size with multi level indirection.

~~~c
[in fs.h]
#define NDIRECT 10 // number of direct address
#define NINDIRECT (BSIZE / sizeof(uint)) // number of single indirect pointer can take direct address.
#define NDUINDIRECT ((BSIZE / sizeof(uint)) * NINDIRECT) // #direct address that du-indirect pointer can have
#define NTRIINDEIECT ((BSIZE / sizeof(uint)) * NDUINDIRECT) // #direct address that tri-indirect pointer can have

#define FREEBLOCKS 1500 // for meta blocks, and default program
#define MAXFILE (FSSIZE - FREEBLOCKS) 

[in defs.h]
#define FSSIZE 40000 // for enough blocks for double, triple indirect.

[structure modification]
in struct inode, struct dnode,
{
    ...
        uint addrs[NDIRECT+3]; // single, double, triple indirection.
}
~~~

~~~c
NINDIRECT => 512/4 = 128;
NDUINDIRECT => 128 * 128 => 2^14 => 16K blocks;
NTRIINDEIECT => 128 ^ 3 => 2^21 => 2M blocks;

one inode can have 10 + 128 + 16K + 2M blocks
    => max file size = about 1GB.
~~~

We can simply calculate the number of address that one inode can have and the result is possible max size of file. But we defined max block number of file system with FSSIZE = 40000, one file can be about 20MB. Also, we need metadata blocks and some default program, I set FREEBLOCK = 1500 for them.  I decided this number with testing [usertests' big file]. When i set under 1500 blocks, it gave me out of block panic.



### Implementation

With structural modification above, we can draw simple picture for our new inode. 

![1](/uploads/bee299b7ee63283653da791717aca3c2/1.png)

With this modified structure, we need to change bmap and itrunc for correctness. 

__bmap__

![2](/uploads/2ff086be7a6d90357db2bb138789f381/2.png)

As we can notice translating flow with picture above, due to recursive form, we can reuse many part of codes. So, i used switch statement which do not break switch until address->data translation.

~~~c
int get_choice(bn); => returns proper index for inode->addr.
    	
choice = get_choice(bn);

	// common procedure. inode->addr[choice + NDIRECT]
  if(choice > -1) {
    if((addr = ip->addrs[NDIRECT + choice]) == 0)
      ip->addrs[NDIRECT + choice] = addr = balloc(ip->dev);
    bn -= NDIRECT;
  }

  switch(choice) {
             // in level 3 [block->block->add->data]
             //             ^^^^^
    case  2: ...
             idx = bn;
             if((addr = a[idx/NDUINDIRECT]) == 0){
               a[idx/NDUINDIRECT] = addr = balloc(ip->dev);
               log_write(bp);
             }
             brelse(bp);
          // << we don't break until add->data
             // in level 2 [block->block->add->data]
             //                    ^^^^^
    case  1: ...
             idx = bn % NDUINDIRECT;
             if((addr = a[idx/NINDIRECT]) == 0){
               a[idx/NINDIRECT] = addr = balloc(ip->dev);
               log_write(bp);
             }
             brelse(bp);
             // in level 1 [block->block->add->data]
             //                           ^^^
    case  0: ...
             idx = bn % NINDIRECT; 
             if((addr = a[idx]) == 0){
               a[idx] = addr = balloc(ip->dev);
               log_write(bp);
             }
             brelse(bp);
             break;
             // in level -1 [direct->data]
    case -1: if((addr = ip->addrs[bn]) == 0)
               ip->addrs[bn] = addr = balloc(ip->dev);
             break;
    default: panic("bmap: out of range");
  }
  return addr;
}

~~~



__itrunc__

Similar with bmap, but slightly different, it needs to be recursive form. (actually bmap is tail recursion.) So i divide direct / single indir / du-indir / tri-indir case and handle each cases. Since tri-indirect case contains every case of others, it will explains how itrunc works.

~~~c
[itrunc]
... 
if(ip->addrs[NDIRECT + 2]){
    bp = bread(ip->dev, ip->addrs[NDIRECT + 2]);
    a = (uint*)bp->data;
    for(j = 0; j < NINDIRECT; ++j){
      if(!a[j])
        continue;
      bp1 = bread(ip->dev, a[j]);
      a1 = (uint*)bp1->data;
      for(k = 0; k < NINDIRECT; ++k){
        if(!a1[k])
          continue;
        bp2 = bread(ip->dev, a1[k]);
        a2 = (uint*)bp2->data; 
        for(s = 0; s < NINDIRECT; ++s)
          if(a2[s])
            bfree(ip->dev, a2[s]); // inode->addr=>blk=>blk=>[add=>data]
        brelse(bp2);
        bfree(ip->dev, a1[k]); // inode->addr=>blk=>[blk=>add]=>data
      }
      brelse(bp1);
      bfree(ip->dev, a[j]); // inode->addr=>[blk=>blk]=>add=>data
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT + 2]);//[inode->addr=>blk]=>blk=>add=>data
  }
...
~~~

from leaf to root, we bfree each blocks.



### test

I did two tests, with usertests.big files for making big file. with 38500 blocks ~ about 2MB.

![3](/uploads/c047fef94a2bbc79be1ec98b64d58803/3.PNG)

And stress tests, for doing 10 big files test sequentially.

![4](/uploads/5d0c3abd9067816fcb602bd04f527609/4.PNG)


---

### Milestone [pread, pwrite]

In last semester, I used pread and pwrite for file system layer of database. Different with read, write system call, pread and pwrite need one more argument, offset. read and write system call changes file's offset but pread and pwrite doesn't. This allow us use multithread for read and write on file. (We must check race condition for write of course.)



I added 2 system call sys_pread, sys_pwrite.

~~~c
int
sys_pread(void)
{
  struct file *f;
  int n, offset;
  char *p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0
  || argptr(1, &p, n) < 0 || argint(3, &offset) < 0)
    return -1;
  return pfileread(f, p, n, offset);
}

int
sys_pwrite(void)
{
  struct file *f;
  int n, offset;
  char *p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0
  || argptr(1, &p, n) < 0 || argint(3, &offset) < 0)
    return -1;
  return pfilewrite(f, p, n, offset);
}
~~~

I got many intuition from how read and write system call implemented. 

~~~c
sys_read => fileread => readi
sys_write => filewrite => writei
    pread, pwrite as well.
sys_pread => pfileread => readi
sys_pwrite => pfilewrite => pwritei (different behaviour with writei)
~~~

pread is compatible with read. The differences between pread and read are 1. offset source (file vs function argument) 2. file offset modification

But for pwrite, it is possible to write larger offset than size of file. This make additional function for implementing pwrite.



~~~c
[pfileread, pfilewrite]
int
pfileread(struct file *f, char *addr, int n, int offset)
{
  int r;

  if(f->readable == 0)
    return -1;
  //pread(2) can't be used in pipe. offset isn't valid in pipe
  if(f->type == FD_PIPE)
    return -1;
  if(f->type == FD_INODE){
    ilock(f->ip);
    //pread 
 =>  r = readi(f->ip, addr, offset, n); // offset instead f->offset
    iunlock(f->ip);
    return r;
  }
  panic("fileread");
}

int
pfilewrite(struct file *f, char *addr, int n, int offset)
{
  int r;

  if(f->writable == 0)
    return -1;
  // no pwrite in pipe. ESPIPE
  if(f->type == FD_PIPE)
    return -1;
  if(f->type == FD_INODE){
    int max = ((MAXOPBLOCKS-1-1-2) / 2) * 512;
    int i = 0;
    while(i < n){
      int n1 = n - i;
      if(n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
 =>     if ((r = pwritei(f->ip, addr + i, offset, n1)) > 0)
        // offset of f does not change.
 =>     offset += r;
      iunlock(f->ip);
      end_op();

      if(r < 0)
        break;
      if(r != n1)
        panic("short filewrite");
      i += r;
    }
    return i == n ? n : -1;
  }
  panic("filewrite");
}

~~~

before pwritei, pread and pwrite is unable in pipe. So i made both function to return -1 when fd is pipe. In man page  of pread, pwrite, it makes ESPIPE signal for calling function with pipe.

~~~c
// writei for pwrite.
int
pwritei(struct inode *ip, char *src, uint off, uint n)
{
  uint tot, m, temp;
  struct buf *bp;

... condition checking
    
  // pwrite over the offset of file. 
  // [file contents][.....][write buffer]
  //               ^       ^            ^
  //  f->offset----|[empty]|--- offset  |-- offset + n
  //  Also, it can be overlapped.

=>  if(ip->size < off){
    int j;
    temp = BSIZE*(ip->size/BSIZE);
    for(j=0;temp < off; ++j){
      bp = bread(ip->dev, bmap(ip, temp));
      log_write(bp);
      brelse(bp);
      temp += BSIZE;
    }
=>  }

    ... actual write
        
  if(n > 0 && off + n > ip->size) {
    ip->size = off;
    iupdate(ip);
  }
  return n;
}

~~~

pwrite can be called with larger offset than size of file. we need to fill zeros between gap. So, for blocks between [file size, offset], I allocated for file before writing in exact offset.



### test

simple check for pwrite that can fill gap between offset and file size

~~~c
  write(fd1, temp, 12); => file1
  pwrite(fd2, temp, 12, 1024*2 + 1 - 12); => file2
~~~

![6_0](/uploads/54c4ec3ecef0bedc0310a8a503deb722/6_0.PNG)


___

Milestone3 => didn't implement



*project 3 ends*

----

마지막 과제까지 모두 구현하고 싶었으나, 상황이 특수하여 모두 구현을 못하였습니다.
죄송하고 참 아쉽습니다.

한 학기 동안 좋은 수업 해주신 교수님, 그리고 혼자 보았다면 정말 어려웠을 코드들을 잘 풀어 설명해주신 조교님들께 감사드립니다.
잘 이끌어주셔서 재밌게 따라간 것 같습니다. 

마지막 sync system call은 스스로 구현해보겠습니다. milestone 1,2만 채점해주세요.

코로나 조심하시구 행복한 시간 되세요. 감사드림니다.

김시우 드림

*OS _fin_*

